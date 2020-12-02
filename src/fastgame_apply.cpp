#include <sys/resource.h>
#include <udisks/udisks.h>
#include <array>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <string>
#include <thread>
#include "ioprio.hpp"
#include "netlink.hpp"
#include "util.hpp"

template <typename T>
void update_system_setting(const std::string& parameter_path, const T& value) {
  std::ofstream f;

  f.open(parameter_path);

  f << value;

  f.close();

  if (f.fail()) {
    util::warning("error writing to " + parameter_path);
  }
}

void update_cpu_frequency_governor(const std::string& name) {
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

void apply_workqueue_affinity(const std::vector<int>& cpu_affinity) {
  int bitmask = 0;

  for (const auto& core_index : cpu_affinity) {
    bitmask += std::pow(2, core_index);
  }

  std::array<std::string, 3> paths{"/sys/devices/virtual/workqueue/cpumask",
                                   "/sys/devices/virtual/workqueue/writeback/cpumask",
                                   "/sys/bus/workqueue/devices/writeback/cpumask"};

  std::ofstream f;

  for (auto& path : paths) {
    f.open(path);

    f << bitmask;

    f.close();

    if (f.fail()) {
      util::warning("error writing the workqueue affinity to: " + path);
    }
  }
}

void apply_udisks_configuration(const boost::property_tree::ptree& root) {
  auto drive_id = root.get<std::string>("disk.udisks.drive-id");
  auto supports_apm = root.get<bool>("disk.udisks.supports-apm", false);
  auto disable_apm = root.get<bool>("disk.udisks.disable-apm", false);
  auto supports_write_cache = root.get<bool>("disk.udisks.supports-write-cache", false);
  auto enable_write_cache = root.get<bool>("disk.udisks.enable-write-cache", false);

  // initializing the udisk client

  GError* error = nullptr;

  auto* client = udisks_client_new_sync(nullptr, &error);

  if (client == nullptr) {
    util::warning(error->message);

    g_error_free(error);

    return;
  }

  auto* objects = g_dbus_object_manager_get_objects(udisks_client_get_object_manager(client));

  while (objects != nullptr) {
    auto* object = UDISKS_OBJECT(objects->data);

    auto* drive = udisks_object_get_drive(object);

    if (drive != nullptr) {
      if (drive_id == udisks_drive_get_id(drive)) {
        GVariantBuilder builder;
        GVariant* value = nullptr;

        g_variant_builder_init(&builder, G_VARIANT_TYPE_VARDICT);

        if (supports_apm) {
          if (disable_apm) {
            g_variant_builder_add(&builder, "{sv}", "ata-apm-level", g_variant_new_int32(255));
          } else {
            g_variant_builder_add(&builder, "{sv}", "ata-apm-level", g_variant_new_int32(127));
          }
        }

        if (supports_write_cache) {
          if (enable_write_cache) {
            g_variant_builder_add(&builder, "{sv}", "ata-write-cache-enabled", g_variant_new_boolean(1));
          } else {
            g_variant_builder_add(&builder, "{sv}", "ata-write-cache-enabled", g_variant_new_boolean(0));
          }
        }

        value = g_variant_builder_end(&builder);

        udisks_drive_call_set_configuration_sync(drive, value, g_variant_new("a{sv}", nullptr), nullptr, nullptr);

        //   util::warning(g_variant_print(value, 1));

        break;
      }
    }

    objects = objects->next;
  }
}

auto main(int argc, char* argv[]) -> int {
  auto input_file = std::filesystem::temp_directory_path() / std::filesystem::path{"fastgame.json"};

  if (!std::filesystem::is_regular_file(input_file)) {
    util::error("fastgame_apply: could not read " + input_file.string());
  }

  std::ofstream cpu_dma_ofstream;
  boost::property_tree::ptree root;
  boost::property_tree::read_json(input_file.string(), root);

  // executable

  auto game_executable = root.get<std::string>("game-executable");

  // cpu

  std::vector<int> game_cpu_affinity;
  std::vector<int> workqueue_cpu_affinity;
  std::vector<int> wineserver_cpu_affinity;

  try {
    for (const auto& c : root.get_child("cpu.game-cores")) {
      int core_index = std::stoi(c.second.data());

      game_cpu_affinity.emplace_back(core_index);
    }

    for (const auto& c : root.get_child("cpu.workqueue-cores")) {
      int core_index = std::stoi(c.second.data());

      workqueue_cpu_affinity.emplace_back(core_index);
    }

    for (const auto& c : root.get_child("cpu.wineserver-cores")) {
      int core_index = std::stoi(c.second.data());

      wineserver_cpu_affinity.emplace_back(core_index);
    }

  } catch (const boost::property_tree::ptree_error& e) {
    util::warning("fastgame_apply: error when parsing the cpu core list");
  }

  auto use_batch_scheduler = root.get<bool>("cpu.use-batch-scheduler", false);
  auto use_realtime_wineserver = root.get<bool>("cpu.use-realtime-wineserver", false);
  int niceness = root.get<int>("cpu.niceness", 0);

  update_system_setting("/proc/sys/kernel/sched_child_runs_first", root.get<bool>("cpu.child-runs-first", false));

  update_cpu_frequency_governor(root.get<std::string>("cpu.frequency-governor", "schedutil"));

  if (root.get<bool>("cpu.use-cpu-dma-latency", false)) {
    cpu_dma_ofstream.open("/dev/cpu_dma_latency");

    cpu_dma_ofstream << 0;
  }

  apply_workqueue_affinity(workqueue_cpu_affinity);

  // disk

  auto disk_device = root.get<std::string>("disk.device");
  auto disk_scheduler = root.get<std::string>("disk.scheduler");
  auto enable_realtime_io_priority = root.get<bool>("disk.enable-realtime-priority", false);
  auto disk_readahead = root.get<int>("disk.readahead", 128);
  auto disk_nr_requests = root.get<int>("disk.nr-requests", 64);
  auto enable_add_random = root.get<bool>("disk.add_random", true);

  update_system_setting(disk_device + "/queue/scheduler", disk_scheduler);
  update_system_setting(disk_device + "/queue/read_ahead_kb", disk_readahead);
  update_system_setting(disk_device + "/queue/nr_requests", disk_nr_requests);
  update_system_setting(disk_device + "/queue/add_random", enable_add_random);

  apply_udisks_configuration(root);

  // amdgpu

  if (util::card_is_amdgpu(0)) {
    update_system_setting("/sys/class/drm/card0/device/power_dpm_force_performance_level",
                          root.get<std::string>("amdgpu.performance-level"));

    int hwmon_index = util::find_hwmon_index(0);
    int power_cap = 1000000 * root.get<int>("amdgpu.power-cap");  // power must be in microwatts
    int gpu_irq = util::get_irq_number("amdgpu");
    int irq_affinity = root.get<int>("amdgpu.irq-affinity", 0);

    update_system_setting("/sys/class/drm/card0/device/hwmon/hwmon" + std::to_string(hwmon_index) + "/power1_cap",
                          power_cap);

    update_system_setting("/proc/irq/" + std::to_string(gpu_irq) + "/smp_affinity_list", irq_affinity);
  }

  // virtual memory

  update_system_setting("/proc/sys/vm/vfs_cache_pressure", root.get<int>("memory.virtual-memory.cache-pressure", 100));

  update_system_setting("/proc/sys/vm/compaction_proactiveness",
                        root.get<int>("memory.virtual-memory.compaction-proactiveness", 20));

  // transparent hugepages

  update_system_setting("/sys/kernel/mm/transparent_hugepage/enabled",
                        root.get<std::string>("memory.transparent-hugepages.enabled"));

  update_system_setting("/sys/kernel/mm/transparent_hugepage/defrag",
                        root.get<std::string>("memory.transparent-hugepages.defrag"));

  update_system_setting("/sys/kernel/mm/transparent_hugepage/shmem_enabled",
                        root.get<std::string>("memory.transparent-hugepages.shmem_enabled"));

  // network

  if (root.get<bool>("network.ipv4.use_tcp_mtu_probing", true)) {
    // Disabled by default, enabled when an ICMP black hole is detected
    update_system_setting("/proc/sys/net/ipv4/tcp_mtu_probing", 1);
  } else {
    update_system_setting("/proc/sys/net/ipv4/tcp_mtu_probing", 0);
  }

  update_system_setting("/proc/sys/net/ipv4/tcp_congestion_control",
                        root.get<std::string>("network.ipv4.tcp_congestion_control", "reno"));

  update_system_setting("/proc/sys/net/ipv4/tcp_max_reordering", root.get<int>("network.ipv4.tcp_max_reordering", 300));

  update_system_setting("/proc/sys/net/ipv4/tcp_probe_interval", root.get<int>("network.ipv4.tcp_probe_interval", 600));

  // starting the netlink server

  int game_pid = -1;
  std::string game_comm;

  auto nl = std::make_unique<Netlink>();

  nl->new_exec.connect([&](int pid, const std::string& comm, const std::string& cmdline, const std::string& exe_path) {
    // std::cout << "new exec: " << comm << "\t" << cmdline << "\t" << std::to_string(pid) << std::endl;

    if (comm == "wineserver") {
      return;
    }

    /*
      Workaround for Proton games run through Pressure-Vessel. More information at
      https://github.com/ValveSoftware/steam-runtime/issues/304
    */

    if (comm == "pressure-vessel") {
      util::apply_cpu_affinity(pid, game_cpu_affinity);

      return;
    }

    if (comm.size() > 4) {
      auto sub_comm = comm.substr(comm.size() - 4);

      if (sub_comm == ":cs0") {  // dxvk or vkd3d ":cs0" thread can not be caught in the fork callback
        setpriority(PRIO_PROCESS, pid, niceness);

        if (enable_realtime_io_priority) {
          ioprio_set_realtime(pid, 7);
        }

        return;
      }
    }

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

      std::cout << "fastgame_apply: " << msg << std::endl;

      setpriority(PRIO_PROCESS, game_pid, niceness);

      if (enable_realtime_io_priority) {
        ioprio_set_realtime(game_pid, 7);
      }
    }
  });

  nl->new_fork.connect([&](int tgid, int child_pid, const std::string& child_comm) {
    if (child_comm == "wineserver") {
      std::cout << "fastgame_apply: wine server pid: " << std::to_string(child_pid) << std::endl;

      if (use_realtime_wineserver) {
        util::set_process_scheduler(child_pid, SCHED_RR, 1);

        std::cout << "fastgame_apply: setting wineserver priority to realtime" << std::endl;
      }

      util::apply_cpu_affinity(child_pid, wineserver_cpu_affinity);
    } else {
      if (tgid == game_pid) {
        // std::cout << "new game fork: " << child_comm << "\t" << std::to_string(child_pid) << std::endl;

        if (use_batch_scheduler) {
          if (child_comm != game_comm) {
            util::set_process_scheduler(child_pid, SCHED_BATCH, 0);

            /*
              Sometimes these ":disk$" threads(vkd3d? dxvk?) use a lot of cpu. It is better to let them use any core
              than force the same affinity as the order game threads
            */

            if (child_comm.find(":disk$") != std::string::npos) {
              util::clear_cpu_affinity(child_pid);
            }
          }
        }

        setpriority(PRIO_PROCESS, child_pid, niceness);

        if (enable_realtime_io_priority) {
          ioprio_set_realtime(child_pid, 7);
        }
      }
    }
  });

  // nl->new_exit.connect([&](int pid) {});

  auto check_lock_file = [&]() {
    while (std::filesystem::is_regular_file(input_file)) {
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    nl->listen = false;
  };

  if (nl->listen) {
    std::thread t(check_lock_file);

    nl->handle_events();  // This is a blocking call. It has to be started at the end

    t.join();
  }

  if (cpu_dma_ofstream.is_open()) {
    cpu_dma_ofstream.close();
  }

  return 0;
}