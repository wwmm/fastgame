#ifndef CGROUPS_HPP
#define CGROUPS_HPP

#include <iostream>
#include "config.hpp"

class Cgroups {
 public:
  Cgroups(Config* config);

  void set(const std::string& game, const int& pid, const bool& is_parent);

 private:
  std::string log_tag = "cgroups: ";
  std::string cpu_dir = "/sys/fs/cgroup/cpu/", parent_dir = cpu_dir + "fastgame_parent",
              children_dir = cpu_dir + "fastgame_children";

  Config* cfg;
};

#endif