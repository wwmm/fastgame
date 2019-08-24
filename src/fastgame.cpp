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

  int game_pid = -1;
  std::string game_name;

  nl->new_exec.connect([&](int pid, std::string name, std::string cmdline) {
    if (game_pid != -1) {
      return;
    }

    for (auto game : cfg->get_games()) {
      bool apply = false;

      if (game.size() > name.size()) {
        auto sub_str = game.substr(0, name.size());

        if (sub_str == name) {
          apply = true;
        }
      } else {
        auto sub_str = name.substr(0, game.size());

        if (sub_str == game) {
          apply = true;
        }
      }

      if (apply) {
        std::cout << "(" + name + ", " + std::to_string(pid) + ", " + cmdline + ")" << std::endl;

        game_name = game;
        game_pid = pid;

        tweaks->apply(game, pid);
      }
    }
  });

  nl->new_fork.connect([&](int tgid, int pid) {
    if (tgid == game_pid) {
      // std::cout << "new thread: (" + game_name + ", " + std::to_string(pid) + ")" << std::endl;

      try {
        auto task_dir = "/proc/" + std::to_string(pid) + "/task";

        for (const auto& entry : fs::directory_iterator(task_dir)) {
          const auto task_pid = entry.path().filename().string();

          tweaks->apply(game_name, std::stoi(task_pid));
        }
      } catch (std::exception& e) {
      }
    }
  });

  nl->new_exit.connect([&](int pid) {
    if (pid == game_pid) {
      std::cout << "No games running." << std::endl;

      game_pid = -1;

      tweaks->remove();
    }
  });

  // This is a blocking call. It has to be started at the end

  if (nl->listen) {
    nl->handle_events();
  }

  return 0;
}
