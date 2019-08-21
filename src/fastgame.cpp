#include <sys/resource.h>
#include <memory>
#include "cmdline_options.hpp"
#include "config.h"
#include "config.hpp"
#include "ioprio.hpp"
#include "netlink.hpp"
#include "scheduler.hpp"

int main(int argc, char* argv[]) {
  auto cmd_options = std::make_unique<CmdlineOptions>(argc, argv);

  auto cfg = std::make_unique<Config>(cmd_options->get_config_file_path());

  auto scheduler = std::make_unique<Scheduler>(cfg.get());

  auto nl = std::make_unique<Netlink>();

  auto games = cfg->get_key_array<std::string>("games");
  auto niceness = cfg->get_key("niceness", 0);
  auto io_class = cfg->get_key<std::string>("io-class", "BE");
  auto io_priority = cfg->get_key("io-priority", 7);

  nl->new_exec.connect([&](int pid, std::string name) {
    // std::cout << "exec: " + std::to_string(pid) + "\t" + name << std::endl;

    for (auto game : games) {
      // std::cout << game << "\t" << name << std::endl;

      if (game.find(name) != std::string::npos) {
        std::cout << name + " is running. Applying tweaks" << std::endl;

        scheduler->set_affinity(pid);
        scheduler->set_policy(pid);

        int r = setpriority(PRIO_PROCESS, pid, niceness);

        if (r != 0) {
          std::cout << "could not set process " + std::to_string(pid) + " niceness" << std::endl;
        }

        r = ioprio_set(pid, io_class, io_priority);

        if (r != 0) {
          std::cout << "could not set process " + std::to_string(pid) + " io class and priority" << std::endl;
        }
      }
    }
  });

  // This is a blocking call. It has to be estart at the end

  if (nl->listen) {
    nl->handle_events();
  }

  return 0;
}
