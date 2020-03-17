#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP

#include <vector>
#include "config.hpp"

void set_policy(const int& pid, const std::string& policy, const int& priority);

void set_affinity(const int& pid, const std::vector<int>& cores);

#endif