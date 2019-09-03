#include "cgroups.hpp"

#include <filesystem>

namespace fs = std::filesystem;

Cgroups::Cgroups(Config* config) : cfg(config) {
  if (!std::filesystem::exists(parent_dir)) {
    fs::create_directory(parent_dir);

    std::cout << log_tag + "created cgroup: " << parent_dir << std::endl;
  } else {
    std::cout << log_tag + "cgroup " + parent_dir + " already exists" << std::endl;
  }

  if (!std::filesystem::exists(children_dir)) {
    fs::create_directory(children_dir);

    std::cout << log_tag + "created cgroup: " << children_dir << std::endl;
  } else {
    std::cout << log_tag + "cgroup " + children_dir + " already exists" << std::endl;
  }

  if (!std::filesystem::exists(cpuset_dir)) {
    fs::create_directory(cpuset_dir);

    std::cout << log_tag + "created cgroup: " << cpuset_dir << std::endl;
  } else {
    std::cout << log_tag + "cgroup " + cpuset_dir + " already exists" << std::endl;
  }
}

void Cgroups::set(const std::string& game, const int& pid, const bool& is_parent) {
  std::ofstream f;

  if (is_parent) {
    auto parent_shares = cfg->get_key("games." + game + ".cgroups.cpu-shares.parent", -1);
    auto children_shares = cfg->get_key("games." + game + ".cgroups.cpu-shares.children", -1);

    if (parent_shares == -1 || children_shares == -1) {
      return;
    }

    // parent

    f.open(parent_dir + "/cpu.shares");

    f << parent_shares;

    f.close();

    // children

    f.open(children_dir + "/cpu.shares");

    f << children_shares;

    f.close();

    // move parent to fastgame_parent

    f.open(parent_dir + "/tasks");

    f << pid;

    f.close();

    parent_pid = pid;
  } else {
    if (pid == parent_pid) {
      return;
    }

    // move child to fastgame_children

    f.open(children_dir + "/tasks");

    f << pid;

    f.close();
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

      f.open(cpuset_dir + "/cpuset.cpus");

      f << cpu_list;

      f.close();

      // mems

      f.open(cpuset_dir + "/cpuset.mems");

      f << 0;

      f.close();

      // load balance

      f.open(cpuset_dir + "/cpuset.sched_load_balance");

      f << 0;

      f.close();

      // tasks

      f.open(cpuset_dir + "/tasks");

      f << pid;

      f.close();
    } else {
      f.open(cpuset_dir + "/tasks");

      f << pid;

      f.close();
    }
  }
}