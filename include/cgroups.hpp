#ifndef CGROUPS_HPP
#define CGROUPS_HPP

#include <iostream>
#include "config.hpp"

class Cgroups {
 public:
  Cgroups(Config* config);

  void config_cpuset(const std::string& game, const int& pid, const bool& is_parent);

 private:
  std::string log_tag = "cgroups: ";
  std::string cpuset_dir = "/sys/fs/cgroup/cpuset", fastgame_dir = cpuset_dir  + "/fastgame";

  int parent_pid = -1;

  Config* cfg;
};

#endif