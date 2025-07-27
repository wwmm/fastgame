#include <fcntl.h>
#include <sched.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <unistd.h>
#include <boost/asio/io_context.hpp>
#include <boost/process/v2/environment.hpp>
#include <boost/process/v2/process.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ptree_fwd.hpp>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include <thread>
#include <vector>
#include "config.h"
#include "ioprio.hpp"
#include "netlink.hpp"
#include "util.hpp"

#ifdef USE_NVIDIA
#include "nvidia/nvidia.hpp"
#endif

using namespace std::string_literals;

template <typename T>
static void update_system_setting(const std::string& parameter_path, const T& value) {
  if (!std::filesystem::exists(parameter_path)) {
    util::debug("the file " + parameter_path + " does not exist!. Aborting the write to it.");

    return;
  }

  std::ofstream f;

  f.open(parameter_path, std::ofstream::trunc);

  f << value;

  f.close();

  if (f.fail()) {
    util::warning("error writing to " + parameter_path);
  }
}

static void update_cpu_frequency_governor(const std::string& name) {
  bool failed = false;
  uint n_cores = std::thread::hardware_concurrency();

  for (uint n = 0; n < n_cores; n++) {
    std::ofstream f;
    auto parameter_path = "/sys/devices/system/cpu/cpu" + std::to_string(n) + "/cpufreq/scaling_governor";

    f.open(parameter_path);

    f << name;

    f.close();

    if (f.fail()) {
      util::warning("fastgame_apply: error writing to " + parameter_path);
      util::warning("fastgame_apply: could not change the frequency governor");

      failed = true;

      break;
    }
  }

  if (!failed) {
    util::debug("fastgame_apply: changed the cpu frequency governor to " + name);
  }
}

static void apply_amdgpu_configuration(const boost::property_tree::ptree& root) {
  auto card_indices = util::get_amdgpu_indices();

  auto boot_vga_idx = util::find_boot_vga(card_indices);

  for (const auto& idx : card_indices) {
    auto is_boot_vga = boot_vga_idx == idx;

    int fd = -1;

    if (!is_boot_vga) {
      fd = util::open_dri_device(idx);
    }

    std::string section = (idx == card_indices.front()) ? "amdgpu" : "amdgpu.card1";

    auto performance_level = root.get<std::string>(section + ".performance-level", "auto");

    update_system_setting("/sys/class/drm/card" + util::to_string(idx) + "/device/power_dpm_force_performance_level",
                          performance_level);

    if (performance_level == "manual") {
      update_system_setting("/sys/class/drm/card" + util::to_string(idx) + "/device/pp_power_profile_mode",
                            root.get<std::string>(section + ".power-profile"));
    }

    int hwmon_index = util::find_hwmon_index(idx);

    int power_cap = 1000000 * root.get<int>(section + ".power-cap");  // power must be in microwatts

    update_system_setting("/sys/class/drm/card" + util::to_string(idx) + "/device/hwmon/hwmon" +
                              util::to_string(hwmon_index) + "/power1_cap",
                          power_cap);

    util::close_dri_device(fd);
  }
}

static void apply_nvidia_configuration(const boost::property_tree::ptree& root) {
#ifdef USE_NVIDIA
  std::unique_ptr<nvidia_wrapper::Nvidia> nv_wrapper = std::make_unique<nvidia_wrapper::Nvidia>();

  if (nv_wrapper->has_gpu()) {
    nv_wrapper->set_powermizer_mode(0, root.get<int>("nvidia.powermize-mode", 0));

    nv_wrapper->nvml->set_power_limit(0, root.get<int>("nvidia.power-limit", 0));

    auto gpu_offset = root.get<int>("nvidia.clock-offset.gpu", 0);
    auto memory_offset = root.get<int>("nvidia.clock-offset.memory", 0);

    nv_wrapper->set_clock_offset(0, gpu_offset, memory_offset);
  }

#endif
}

static void update_pcie_aspm(const boost::property_tree::ptree& root) {
  auto card_indices = util::get_amdgpu_indices();

  auto boot_vga_idx = util::find_boot_vga(card_indices);

  int fd = -1;

  for (auto n : card_indices) {
    auto is_boot_vga = boot_vga_idx == n;

    if (!is_boot_vga) {
      fd = util::open_dri_device(n);

      break;
    }
  }

  update_system_setting("/sys/module/pcie_aspm/parameters/policy",
                        root.get<std::string>("cpu.pcie-aspm-policy", "default"));

  util::close_dri_device(fd);
}

static void apply_sched_ext(const boost::property_tree::ptree& root) {
  auto enable = root.get<bool>("scx_sched.enable", false);
  auto scheduler = root.get<std::string>("scx_sched.scheduler");

  if (!enable || scheduler.empty()) {
    return;
  }

  // arguments

  std::string arguments;

  try {
    for (const auto& c : root.get_child("scx_sched.arguments")) {
      arguments.append(c.second.data());
    }
  } catch (const boost::property_tree::ptree_error& e) {
    util::warning("error when paring scx_sched arguments list");
  }

  boost::asio::io_context ctx;

  auto exe = boost::process::environment::find_executable("scxctl");

  if (exe.empty()) {
    util::warning("Could not find the command scxctl");

    return;
  }

  auto load_sched = boost::process::process(ctx, exe, {"start", "--sched " + scheduler, "--args=" + arguments});

  load_sched.wait();
}

static void disable_scx_sched() {
  boost::asio::io_context ctx;

  auto exe = boost::process::environment::find_executable("scxctl");

  if (exe.empty()) {
    util::warning("Could not find the command scxctl");

    return;
  }

  auto load_sched = boost::process::process(ctx, exe, {"stop"});
}

auto main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) -> int {
  auto input_file = std::filesystem::temp_directory_path() / std::filesystem::path{"fastgame.json"};

  if (!std::filesystem::is_regular_file(input_file)) {
    util::fatal("fastgame_apply: could not read " + input_file.string());
  }

  boost::property_tree::ptree root;
  boost::property_tree::read_json(input_file.string(), root);

  // executable

  auto game_executable = root.get<std::string>("game-executable");

  // cpu

  std::vector<int> game_cpu_affinity = util::parse_affinity_str(root.get<std::string>("cpu.game-cores", ""));
  std::vector<int> wineserver_cpu_affinity =
      util::parse_affinity_str(root.get<std::string>("cpu.wineserver-cores", ""));

  auto use_batch_scheduler = root.get<bool>("cpu.use-batch-scheduler", false);
  auto use_realtime_wineserver = root.get<bool>("cpu.use-realtime-wineserver", false);
  int niceness = root.get<int>("cpu.niceness", 0);
  int autogroup_niceness = root.get<int>("cpu.autogroup-niceness", 0);
  double sched_runtime = root.get<double>("cpu.sched-runtime", util::get_sched_runtime(0, 0));

  update_system_setting("/proc/sys/kernel/watchdog", root.get<bool>("cpu.enable-watchdog", true));

  update_pcie_aspm(root);

  update_cpu_frequency_governor(root.get<std::string>("cpu.frequency-governor", "schedutil"));

  update_system_setting("/sys/module/workqueue/parameters/default_affinity_scope",
                        root.get<std::string>("cpu.workqueue.affinity-scope", "cache"));

  update_system_setting("/sys/module/workqueue/parameters/cpu_intensive_thresh_us",
                        root.get<int>("cpu.workqueue.cpu-intensive-threshold", 10000));

  update_system_setting("/proc/sys/kernel/split_lock_mitigate",
                        root.get<bool>("cpu.enable-split-lock-mitigation", true));

  int cpu_dma_latency_fd = -1;

  if (root.get<bool>("cpu.use-cpu-dma-latency", false)) {
    uint32_t target = root.get<uint32_t>("cpu.cpu-dma-latency", 0);

    cpu_dma_latency_fd = open("/dev/cpu_dma_latency", O_RDWR);

    if (cpu_dma_latency_fd < 0) {
      util::warning("failed to open /dev/cpu_dma_latency");
    }

    if (write(cpu_dma_latency_fd, &target, sizeof(target)) < 0) {
      util::warning("failed to write zero to /dev/cpu_dma_latency");
    } else {
      util::info("writing zero to /dev/cpu_dma_latency");
    }
  }

  // disk
  auto enable_realtime_io_priority = false;

  {
    auto mounting_path = root.get<std::string>("disk.mounting-path");

    auto disk_device = util::mounting_path_to_sys_class_path(mounting_path);

    if (!disk_device.empty()) {
      auto disk_readahead = root.get<int>("disk.readahead", 128);
      auto enable_add_random = root.get<bool>("disk.add_random", true);
      auto disk_scheduler = root.get<std::string>("disk.scheduler");
      auto disk_nr_requests = root.get<int>("disk.nr-requests", 64);
      auto disk_rq_affinity = root.get<int>("disk.rq-affinity", 1);
      auto disk_nomerges = root.get<int>("disk.nomerges", 0);
      auto disk_wbt_lat_usec = root.get<int>("disk.wbt-lat-usec", -1);

      enable_realtime_io_priority = root.get<bool>("disk.enable-realtime-priority", false);

      update_system_setting(disk_device + "/queue/add_random", enable_add_random);
      update_system_setting(disk_device + "/queue/scheduler", disk_scheduler);
      update_system_setting(disk_device + "/queue/read_ahead_kb", disk_readahead);
      update_system_setting(disk_device + "/queue/nr_requests", disk_nr_requests);
      update_system_setting(disk_device + "/queue/nomerges", disk_nomerges);
      update_system_setting(disk_device + "/queue/wbt_lat_usec", disk_wbt_lat_usec);
      update_system_setting(disk_device + "/queue/rq_affinity", disk_rq_affinity);
    }
  }

  apply_sched_ext(root);

  // amdgpu

  apply_amdgpu_configuration(root);

  // nvidia

  apply_nvidia_configuration(root);

  // virtual memory

  update_system_setting("/proc/sys/vm/compact_memory", 1);

  update_system_setting("/proc/sys/vm/swappiness", root.get<int>("memory.virtual-memory.swappiness", 100));

  update_system_setting("/proc/sys/vm/page-cluster", root.get<int>("memory.virtual-memory.page-cluster", 3));

  update_system_setting("/proc/sys/vm/vfs_cache_pressure", root.get<int>("memory.virtual-memory.cache-pressure", 100));

  update_system_setting("/proc/sys/vm/compaction_proactiveness",
                        root.get<int>("memory.virtual-memory.compaction-proactiveness", 20));

  update_system_setting("/proc/sys/vm/extfrag_threshold",
                        root.get<int>("memory.virtual-memory.extfrag-threshold", 500));

  update_system_setting("/proc/sys/vm/min_free_kbytes", root.get<int>("memory.virtual-memory.min-free-kbytes", 67584));

  update_system_setting("/proc/sys/vm/min_unmapped_ratio",
                        root.get<int>("memory.virtual-memory.min-unmapped-ratio", 1));

  update_system_setting("/proc/sys/vm/min_slab_ratio", root.get<int>("memory.virtual-memory.min-slab-ratio", 5));

  update_system_setting("/proc/sys/vm/page_lock_unfairness",
                        root.get<int>("memory.virtual-memory.page-lock-unfairness", 5));

  update_system_setting("/proc/sys/vm/percpu_pagelist_high_fraction",
                        root.get<int>("memory.virtual-memory.percpu-pagelist-high-fraction", 0));

  update_system_setting("/proc/sys/vm/zone_reclaim_mode", root.get<int>("memory.virtual-memory.zone-reclaim-mode", 0));

  // mglru

  update_system_setting("/sys/kernel/mm/lru_gen/min_ttl_ms", root.get<int>("memory.mglru.min_ttl_ms", 0));

  // transparent hugepages

  update_system_setting("/sys/kernel/mm/transparent_hugepage/enabled",
                        root.get<std::string>("memory.transparent-hugepages.enabled"));

  update_system_setting("/sys/kernel/mm/transparent_hugepage/defrag",
                        root.get<std::string>("memory.transparent-hugepages.defrag"));

  update_system_setting("/sys/kernel/mm/transparent_hugepage/shmem_enabled",
                        root.get<std::string>("memory.transparent-hugepages.shmem_enabled"));

  update_system_setting("/sys/kernel/mm/transparent_hugepage/khugepaged/scan_sleep_millisecs",
                        root.get<int>("memory.transparent-hugepages.scan-sleep", 10000));

  update_system_setting("/sys/kernel/mm/transparent_hugepage/khugepaged/alloc_sleep_millisecs",
                        root.get<int>("memory.transparent-hugepages.alloc-sleep", 60000));

  update_system_setting("/proc/sys/vm/hugetlb_optimize_vmemmap",
                        root.get<bool>("memory.transparent-hugepages.hugetlb-optimize-vmemmap", false));

  // starting the netlink server

  int game_pid = -1;
  std::string game_comm;

  auto nl = std::make_unique<Netlink>();

  nl->new_exec.connect([&](int pid, const std::string& comm, const std::string& cmdline, const std::string& exe_path) {
    // std::cout << "new exec: " << comm << "\t" << cmdline << "\t" << std::to_string(pid) << std::endl;

    if (comm == "wineserver") {
      return;
    }

    util::apply_cpu_affinity(pid, game_cpu_affinity);

    auto apply = false;
    auto path_comm = std::filesystem::path(comm);

    if (game_executable == comm || game_executable == path_comm.stem().string()) {
      apply = true;
    } else if (comm.size() == 15) {  // comm is larger than 15 characters and was truncated by the kernel
      auto sub_str = game_executable.substr(0, 15);

      if (comm == sub_str) {
        apply = true;
      }
    }

    if (apply) {
      game_pid = pid;
      game_comm = comm;

      std::string msg = "(";

      msg.append(std::to_string(pid) + ", " + comm + ", ");
      msg.append(exe_path + ", ");
      msg.append(cmdline + ")");

      util::info(msg);

      setpriority(PRIO_PROCESS, game_pid, niceness);

      update_system_setting("/proc/" + util::to_string(game_pid) + "/autogroup", autogroup_niceness);

      if (use_batch_scheduler) {
        util::set_sched_runtime(game_pid, sched_runtime, SCHED_BATCH, niceness, 0);

      } else {
        util::set_sched_runtime(game_pid, sched_runtime, SCHED_OTHER, niceness, 0);
      }

      if (enable_realtime_io_priority) {
        ioprio_set_realtime(game_pid, 7);
      }
    }
  });

  nl->new_fork.connect([&](int tgid, int child_pid, const std::string& child_comm) {
    if (child_comm == "wineserver") {
      util::info("fastgame_apply: wine server pid: " + util::to_string(child_pid));

      if (use_realtime_wineserver) {
        util::set_process_scheduler(child_pid, SCHED_RR, 1);

        util::info("fastgame_apply: setting wineserver priority to realtime");
      }

      util::apply_cpu_affinity(child_pid, wineserver_cpu_affinity);
    } else {
      if (tgid == game_pid) {
        // std::cout << "new game fork: " << child_comm << "\t" << std::to_string(child_pid) << std::endl;

        if (use_batch_scheduler) {
          util::set_process_scheduler(child_pid, SCHED_BATCH, 0);

          util::set_sched_runtime(child_pid, sched_runtime, SCHED_BATCH, niceness, 0);
        } else {
          util::set_sched_runtime(child_pid, sched_runtime, SCHED_OTHER, niceness, 0);
        }

        setpriority(PRIO_PROCESS, child_pid, niceness);

        if (enable_realtime_io_priority) {
          ioprio_set_realtime(child_pid, 7);
        }
      }
    }
  });

  // nl->new_exit.connect([&](int pid) {});

  if (nl->listen) {
    nl->handle_events();  // This is a blocking call. It has to be started at the end

    util::info("Netlink event monitor finished.");
  }

  if (cpu_dma_latency_fd != -1) {
    util::info("closing /dev/cpu_dma_latency");

    close(cpu_dma_latency_fd);
  }

  disable_scx_sched();

  return 0;
}