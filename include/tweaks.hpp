#ifndef TWEAKS_HPP
#define TWEAKS_HPP

#include <iostream>
#include <memory>
#include "config.hpp"
#include "nvidia.hpp"
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
  std::unique_ptr<Nvidia> nvidia;

  void change_cpu_governor(const std::string& name);
  void change_disk_scheduler(const std::string& name);
  void change_disk_read_ahead(const int& value);
  void change_iopriority(const std::string& game, const int& pid);
  void change_niceness(const std::string& game, const int& pid);
  void change_cpu_scheduler_affinity_and_policy(const std::string& game, const int& pid);
};

#endif