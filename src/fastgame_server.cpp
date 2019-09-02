#include <string.h>
#include <filesystem>
#include <memory>
#include "cmdline_options.hpp"
#include "config.h"
#include "config.hpp"
#include "netlink.hpp"
#include "tweaks.hpp"

namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
  auto cmd_options = std::make_unique<CmdlineOptions>(argc, argv);

  auto cfg = std::make_unique<Config>(cmd_options->get_config_file_path());

  auto tweaks = std::make_unique<Tweaks>(cfg.get());

  auto nl = std::make_unique<Netlink>();

  std::vector<std::pair<std::string, int>> pid_list;

  nl->new_exec.connect([&](int pid, std::string name, std::string cmdline) {
    for (auto game : cfg->get_games()) {
      bool apply = false;

      if (game == name) {
        apply = true;
      } else if (game.size() < name.size()) {
        auto sub_str = name.substr(0, game.size());

        if (sub_str == game) {
          apply = true;
        }
      } else {
        auto sub_str = game.substr(0, name.size());

        if (sub_str == name) {
          apply = true;
        }
      }

      if (apply) {
        tweaks->apply_process(game, pid, true);

        if (pid_list.size() == 0) {
          tweaks->apply_global();
        }

        pid_list.push_back(std::pair(game, pid));

        std::cout << "(" + name + ", " + std::to_string(pid) + ", " + cmdline + ")" << std::endl;
      }
    }
  });

  nl->new_fork.connect([&](int tgid, int pid) {
    for (auto& p : pid_list) {
      if (tgid == p.second) {
        // std::cout << "new thread: (" + p.first + ", " + std::to_string(pid) + ")" << std::endl;

        /*
          The loop below is probably unnecessary for native games. But wine games needs it because by the time wine
          updates /proc/pid/comm with the game process name a few child threads may have been already started.
        */

        try {
          auto task_dir = "/proc/" + std::to_string(pid) + "/task";

          for (const auto& entry : fs::directory_iterator(task_dir)) {
            const auto task_pid = entry.path().filename().string();

            tweaks->apply_process(p.first, std::stoi(task_pid), false);
          }
        } catch (std::exception& e) {
        }
      }
    }
  });

  nl->new_exit.connect([&](int pid) {
    bool remove_element = false;

    for (auto& p : pid_list) {
      if (p.second == pid) {
        remove_element = true;

        break;
      }
    }

    if (remove_element) {
      // std::cout << "exit: " + std::to_string(pid) << std::endl;

      pid_list.erase(std::remove_if(pid_list.begin(), pid_list.end(), [=](auto p) { return p.second == pid; }),
                     pid_list.end());

      if (pid_list.size() == 0) {
        std::cout << "No games running." << std::endl;

        tweaks->remove();
      }
    }
  });

  // This is a blocking call. It has to be started at the end

  if (nl->listen) {
    nl->handle_events();
  }

  return 0;
}
