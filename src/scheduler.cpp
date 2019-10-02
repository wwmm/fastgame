#include "scheduler.hpp"
#include <sched.h>
#include <iostream>
#include <string>

Scheduler::Scheduler() {}

void Scheduler::set_affinity(const int& pid, const std::vector<int>& cores) {
  if (cores.size() > 0) {
    cpu_set_t mask;

    CPU_ZERO(&mask);  // Initialize it all to 0, i.e. no CPUs selected.

    for (auto& core : cores) {
      CPU_SET(core, &mask);  // set the bit that represents the core.
    }

    if (sched_setaffinity(pid, sizeof(cpu_set_t), &mask) < 0) {
      // std::cout << "could not set process " + std::to_string(pid) + " cpu affinity" << std::endl;
    }
  }
}

void Scheduler::set_policy(const int& pid, const std::string& policy, const int& priority) {
  int policy_index;
  sched_param policy_params;

  policy_params.sched_priority = priority;

  if (policy == "SCHED_BATCH") {
    policy_index = SCHED_BATCH;
  } else if (policy == "SCHED_RR") {
    policy_index = SCHED_RR;
  } else if (policy == "SCHED_FIFO") {
    policy_index = SCHED_FIFO;
  } else if (policy == "SCHED_ISO") {
    policy_index = SCHED_ISO;
  } else if (policy == "SCHED_DEADLINE") {
    policy_index = SCHED_DEADLINE;
  } else {
    policy_index = SCHED_OTHER;
  }

  if (sched_setscheduler(pid, policy_index, &policy_params) == -1) {
    // std::cout << "could not set process " + std::to_string(pid) + " scheduler policy" << std::endl;
  }
}