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

 private:
  std::string log_tag = "tweaks: ";

  Config* cfg;
  std::unique_ptr<Scheduler> scheduler;
};

#endif