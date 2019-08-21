#include "scheduler.hpp"
#include <sched.h>
#include <iostream>
#include <string>

Scheduler::Scheduler(Config* config) : cfg(config) {
  auto affinity_cores = cfg->get_key_array<int>("affinity-cores");

  CPU_ZERO(&mask); /* Initialize it all to 0, i.e. no CPUs selected. */

  for (auto& core : affinity_cores) {
    CPU_SET(core, &mask); /* set the bit that represents core 7. */
  }

  std::string policy = cfg->get_key<std::string>("scheduler-policy", "SCHED_OTHER");
  policy_params.sched_priority = cfg->get_key("scheduler-policy-priority", 0);

  if (policy == "SCHED_BATCH") {
    policy_index = SCHED_BATCH;
  } else if (policy == "SCHED_RR") {
    policy_index = SCHED_RR;
  } else if (policy == "SCHED_ISO") {
    policy_index = SCHED_ISO;
  } else if (policy == "SCHED_DEADLINE") {
    policy_index = SCHED_DEADLINE;
  } else {
    policy_index = SCHED_OTHER;
  }
}

void Scheduler::set_affinity(const int& pid) {
  int res = sched_setaffinity(pid, sizeof(cpu_set_t), &mask);

  if (res == -1) {
    std::cout << "could not set process " + std::to_string(pid) + " cpu affinity" << std::endl;
  }
}

void Scheduler::set_policy(const int& pid) {
  int res = sched_setscheduler(pid, policy_index, &policy_params);

  if (res == -1) {
    std::cout << "could not set process " + std::to_string(pid) + " scheduler policy" << std::endl;
  }
}