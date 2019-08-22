#include <memory>
#include "cmdline_options.hpp"
#include "config.h"
#include "config.hpp"
#include "netlink.hpp"
#include "tweaks.hpp"

int main(int argc, char* argv[]) {
  auto cmd_options = std::make_unique<CmdlineOptions>(argc, argv);

  auto cfg = std::make_unique<Config>(cmd_options->get_config_file_path());

  auto tweaks = std::make_unique<Tweaks>(cfg.get());

  auto nl = std::make_unique<Netlink>();

  std::vector<int> pid_list;

  nl->new_exec.connect([&](int pid, std::string name, std::string cmdline) {
    // std::cout << "exec: " + std::to_string(pid) + "\t" + name + "\t" + cmdline << std::endl;

    for (auto game : cfg->get_games()) {
      bool apply = false;

      /*
        name can be shorter than the real process name because the kernel limits the number of characters in
        /proc/pid/comm
      */

      auto game_sub_str = game.substr(0, name.size());

      if (game_sub_str == name) {
        apply = true;
      }

      if (apply) {
        std::cout << "(" + name + ", " + std::to_string(pid) + ", " + cmdline + ")" << std::endl;

        pid_list.push_back(pid);

        tweaks->apply(game, pid);
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
      std::cout << "exit: " + std::to_string(pid) << std::endl;

      pid_list.erase(std::remove(pid_list.begin(), pid_list.end(), pid), pid_list.end());

      if (pid_list.size() == 0) {
        std::cout << "No games running." << std::endl;
      }
    }
  });

  // This is a blocking call. It has to be estart at the end

  if (nl->listen) {
    nl->handle_events();
  }

  return 0;
}
