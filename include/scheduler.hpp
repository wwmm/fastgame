#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP

#include "config.hpp"

class Scheduler {
 public:
  Scheduler(Config* config);

  void set_affinity(const int& pid);

  void set_policy(const int& pid);

 private:
  Config* cfg;

  cpu_set_t mask;
  int policy_index;
  sched_param policy_params;
};

#endif