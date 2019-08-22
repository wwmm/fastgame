#include <sys/resource.h>
#include <algorithm>
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

  // Reading config

  auto games = cfg->get_key_array<std::string>("games");
  auto niceness = cfg->get_key("niceness", 0);
  auto io_class = cfg->get_key<std::string>("io-class", "BE");
  auto io_priority = cfg->get_key("io-priority", 7);

  // game pid list

  std::vector<int> pid_list;

  nl->new_exec.connect([&](int pid, std::string name, std::string cmdline) {
    // std::cout << "exec: " + std::to_string(pid) + "\t" + name << std::endl;

    for (auto game : games) {
      bool apply_policies = false;

      if (game == name) {
        apply_policies = true;
      } else if (cmdline.find(game) != std::string::npos) {
        apply_policies = true;
      }

      if (apply_policies) {
        std::cout << "(" + name + ", " + std::to_string(pid) + ")" + " is running. Applying tweaks" << std::endl;

        pid_list.push_back(pid);

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

  nl->new_exit.connect([&](int pid) {
    bool remove_element = false;

    for (auto& p : pid_list) {
      if (p == pid) {
        remove_element = true;

        break;
      }
    }

    if (remove_element) {
      // std::cout << "exit: " + std::to_string(pid) << std::endl;

      pid_list.erase(std::remove(pid_list.begin(), pid_list.end(), pid), pid_list.end());

      if (pid_list.size() == 0) {
        std::cout << "No games running. Reverting tweaks." << std::endl;
      }
    }
  });

  // This is a blocking call. It has to be estart at the end

  if (nl->listen) {
    nl->handle_events();
  }

  return 0;
}
