#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <chrono>
#include <filesystem>
#include <thread>
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

  update_system_setting("/proc/sys/kernel/sched_child_runs_first", root.get<bool>("cpu.child-runs-first"));

  update_cpu_frequency_governor(root.get<std::string>("cpu.frequency-governor"));

  // amdgpu

  update_system_setting("/sys/class/drm/card0/device/power_dpm_force_performance_level",
                        root.get<std::string>("amdgpu.performance-level"));

  int hwmon_index = util::find_hwmon_index(0);
  int power_cap = 1000000 * root.get<int>("amdgpu.power-cap");  // power must be in microwatts

  update_system_setting("/sys/class/drm/card0/device/hwmon/hwmon" + std::to_string(hwmon_index) + "/power1_cap",
                        power_cap);

  // virtual memory

  update_system_setting("/proc/sys/vm/vfs_cache_pressure", root.get<int>("memory.virtual-memory.cache-pressure"));

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

    util::info(comm);

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

      // cpu affinity

      cpu_set_t mask;

      CPU_ZERO(&mask);  // Initialize it all to 0, i.e. no CPUs selected.

      for (const auto& core_index : cpu_affinity) {
        CPU_SET(core_index, &mask);
      }

      if (sched_setaffinity(game_pid, sizeof(cpu_set_t), &mask) < 0) {
        util::warning("fastgame_apply: could not set the process cpu affinity");
      }
    }
  });

  nl->new_fork.connect([&](int tgid, int child_pid, const std::string& child_comm) {
    if (child_comm == "wineserver") {
      util::info("fastgame_apply: wine server pid: " + std::to_string(child_pid));
    } else {
      if (tgid == game_pid && child_pid != tgid) {
      }
    }
  });

  nl->new_exit.connect([&](int pid) {});

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

  return 0;
}