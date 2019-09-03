#ifndef CGROUPS_HPP
#define CGROUPS_HPP

#include <iostream>
#include "config.hpp"

class Cgroups {
 public:
  Cgroups(Config* config);

  void set(const std::string& game, const int& pid, const bool& is_parent);

  void config_cpuset(const std::string& game, const int& pid, const bool& is_parent);

 private:
  std::string log_tag = "cgroups: ";
  std::string cpu_dir = "/sys/fs/cgroup/cpu/", parent_dir = cpu_dir + "fastgame_parent",
              children_dir = cpu_dir + "fastgame_children", cpuset_dir = "/sys/fs/cgroup/cpuset/fastgame";

  int parent_pid = -1;

  Config* cfg;
};

#endif