#include <sys/resource.h>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <chrono>
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

  std::vector<int> cpu_affinity;

  try {
    for (const auto& c : root.get_child("cpu.cores")) {
      int core_index = std::stoi(c.second.data());

      cpu_affinity.emplace_back(core_index);
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

  // disk

  auto disk_device = root.get<std::string>("disk.device");
  auto disk_scheduler = root.get<std::string>("disk.scheduler");
  auto enable_realtime_io_priority = root.get<bool>("disk.enable-realtime-priority", false);
  auto disk_readahead = root.get<int>("disk.readahead", 128);
  auto disk_nr_requests = root.get<int>("disk.nr-requests", 64);

  update_system_setting(disk_device + "/queue/scheduler", disk_scheduler);
  update_system_setting(disk_device + "/queue/read_ahead_kb", disk_readahead);
  update_system_setting(disk_device + "/queue/nr_requests", disk_nr_requests);

  // amdgpu

  update_system_setting("/sys/class/drm/card0/device/power_dpm_force_performance_level",
                        root.get<std::string>("amdgpu.performance-level"));

  int hwmon_index = util::find_hwmon_index(0);
  int power_cap = 1000000 * root.get<int>("amdgpu.power-cap");  // power must be in microwatts

  update_system_setting("/sys/class/drm/card0/device/hwmon/hwmon" + std::to_string(hwmon_index) + "/power1_cap",
                        power_cap);

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

  // starting the netlink server

  int game_pid = -1;

  auto nl = std::make_unique<Netlink>();

  nl->new_exec.connect([&](int pid, const std::string& comm, const std::string& cmdline, const std::string& exe_path) {
    if (comm == "wineserver") {
      return;
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

      std::string msg = "(";

      msg.append(std::to_string(pid) + ", " + comm + ", ");
      msg.append(exe_path + ", ");
      msg.append(cmdline + ")");

      std::cout << "fastgame_apply: " << msg << std::endl;

      util::apply_cpu_affinity(game_pid, cpu_affinity);

      if (use_batch_scheduler) {
        util::set_process_scheduler(game_pid, SCHED_BATCH, 0);
      }

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
        util::apply_cpu_affinity(child_pid, cpu_affinity);

        util::set_process_scheduler(child_pid, SCHED_RR, 1);

        std::cout << "fastgame_apply: setting wineserver priority to realtime" << std::endl;
      }
    } else {
      if (tgid == game_pid) {
        /*
          For some reason not all of the children can be intercepted here. So we loop over all children whenever a new
          one is created. This way we are able to apply settings to all of them.
        */

        try {
          auto task_dir = "/proc/" + std::to_string(game_pid) + "/task";

          for (const auto& entry : std::filesystem::directory_iterator(task_dir)) {
            const auto task_pid = std::stoi(entry.path().filename().string());

            util::apply_cpu_affinity(task_pid, cpu_affinity);

            if (use_batch_scheduler) {
              util::set_process_scheduler(task_pid, SCHED_BATCH, 0);
            }

            setpriority(PRIO_PROCESS, game_pid, niceness);

            if (enable_realtime_io_priority) {
              ioprio_set_realtime(game_pid, 7);
            }
          }
        } catch (std::exception& e) {
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