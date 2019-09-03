#include "cgroups.hpp"

#include <filesystem>

namespace fs = std::filesystem;

Cgroups::Cgroups(Config* config) : cfg(config) {
  if (!std::filesystem::exists(fastgame_dir)) {
    fs::create_directory(fastgame_dir);

    std::cout << log_tag + "created cgroup: " << fastgame_dir << std::endl;
  } else {
    std::cout << log_tag + "cgroup " + fastgame_dir + " already exists" << std::endl;
  }
}

void Cgroups::config_cpuset(const std::string& game, const int& pid, const bool& is_parent) {
  auto cpus = cfg->get_key_array<int>("games." + game + ".cpu-affinity");

  if (cpus.size() > 0) {
    std::ofstream f;

    if (is_parent) {
      auto cpu_list = std::to_string(cpus[0]);

      for (uint n = 1; n < cpus.size(); n++) {
        cpu_list += "," + std::to_string(cpus[n]);
      }

      // cpus

      f.open(fastgame_dir + "/cpuset.cpus");

      f << cpu_list;

      f.close();

      // mems

      f.open(fastgame_dir + "/cpuset.mems");

      f << 0;

      f.close();

      // load balance

      // f.open(fastgame_dir + "/fastgame/cpuset.sched_load_balance");

      // f << 0;

      // f.close();

      // tasks

      f.open(fastgame_dir + "/tasks");

      f << pid;

      f.close();
    } else {
      f.open(fastgame_dir + "/tasks");

      f << pid;

      f.close();
    }
  }
}