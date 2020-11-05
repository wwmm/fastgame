#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
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

  // cpu

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

  auto nl = std::make_unique<Netlink>();

  nl->new_exec.connect([&](int pid, const std::string& comm, const std::string& cmdline, const std::string& exe_path) {
    if (comm == "wineserver") {
      return;
    }

    auto apply = false;
    auto path_comm = std::filesystem::path(comm);
    std::string game = "game.exe";

    if (game == comm || game == path_comm.stem().string()) {
      apply = true;
    } else if (comm.size() == 15) {  // comm is larger than 15 characters and was truncated by the kernel
      auto sub_str = game.substr(0, 15);

      if (comm == sub_str) {
        apply = true;
      }
    }

    if (apply) {
      std::string msg = "(";

      msg.append(std::to_string(pid) + ", " + comm + ", ");
      msg.append(exe_path + ", ");
      msg.append(cmdline + ")");

      util::info("fastgame_apply: " + msg);
    }
  });

  nl->new_exit.connect([&](int pid) {});

  if (nl->listen) {
    nl->lock_file = input_file;

    nl->handle_events();  // This is a blocking call. It has to be started at the end
  }

  return 0;
}