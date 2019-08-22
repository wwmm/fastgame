#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP

#include <vector>
#include "config.hpp"

class Scheduler {
 public:
  Scheduler();

  void set_affinity(const int& pid, const std::vector<int>& cores);

  void set_policy(const int& pid, const std::string& policy, const int& priority);

 private:
};

#endif