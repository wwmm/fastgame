#include "tweaks.hpp"
#include <sys/resource.h>
#include "ioprio.hpp"

Tweaks::Tweaks(Config* config) : cfg(config), scheduler(std::make_unique<Scheduler>()) {}

void Tweaks::apply(const std::string& game, const int& pid) {
  auto affinity_cores = cfg->get_key_array<int>(game + ".affinity-cores");
  auto sched_policy = cfg->get_key<std::string>(game + ".scheduler-policy", "SCHED_OTHER");
  auto sched_priority = cfg->get_key(game + ".scheduler-policy-priority", 0);
  auto niceness = cfg->get_key(game + ".niceness", 0);
  auto io_class = cfg->get_key<std::string>(game + ".io-class", "BE");
  auto io_priority = cfg->get_key(game + ".io-priority", 7);

  scheduler->set_affinity(pid, affinity_cores);
  scheduler->set_policy(pid, sched_policy, sched_priority);

  int r = setpriority(PRIO_PROCESS, pid, niceness);

  if (r != 0) {
    std::cout << log_tag + "could not set process " + std::to_string(pid) + " niceness" << std::endl;
  }

  r = ioprio_set(pid, io_class, io_priority);

  if (r != 0) {
    std::cout << log_tag + "could not set process " + std::to_string(pid) + " io class and priority" << std::endl;
  }

  // std::cout << log_tag + "applied tweaks for (" + game + ", " + std::to_string(pid) + ")" << std::endl;
}