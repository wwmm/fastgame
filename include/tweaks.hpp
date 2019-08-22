#ifndef TWEAKS_HPP
#define TWEAKS_HPP

#include <iostream>
#include <memory>
#include "config.hpp"
#include "scheduler.hpp"

class Tweaks {
 public:
  Tweaks(Config* config);

  void apply(const std::string& game, const int& pid);

  void remove();

 private:
  std::string log_tag = "tweaks: ";

  Config* cfg;
  std::unique_ptr<Scheduler> scheduler;

  void change_governor(const std::string& name);
  void change_iopriority(const std::string& game, const int& pid);
  void change_niceness(const std::string& game, const int& pid);
  void change_scheduler_affinity_and_policy(const std::string& game, const int& pid);
};

#endif