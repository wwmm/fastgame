#include "cmdline_options.hpp"
#include "config.h"
#include "config.hpp"
#include "netlink.hpp"
#include "tweaks.hpp"

namespace fs = std::filesystem;

auto main(int argc, char* argv[]) -> int {
  auto cmd_options = std::make_unique<CmdlineOptions>(argc, argv);

  auto cfg = std::make_unique<Config>(cmd_options->get_config_file_path());

  auto tweaks = std::make_unique<Tweaks>(cfg.get());

  auto nl = std::make_unique<Netlink>();

  std::string game_name;
  int game_pid = -1;

  nl->new_exec.connect([&](int pid, const std::string& comm, const std::string& cmdline, const std::string& exe_path) {
    if (comm == "wineserver") {
      return;
    }

    for (const auto& pair : cfg->get_games()) {
      auto apply = false;
      auto path_comm = fs::path(comm);
      auto game = pair.first;

      if (game == comm || game == path_comm.stem().string()) {
        apply = true;
      } else if (comm.size() == 15) {  // comm is larger than 15 characters and was truncated by the kernel
        auto sub_str = game.substr(0, 15);

        if (comm == sub_str) {
          apply = true;
        }
      }

      if (apply) {
        game_pid = pid;
        game_name = game;

        tweaks->game_parent_pid = pid;

        tweaks->apply_process(game, pid);

        tweaks->apply_global();

        std::string msg = "(";

        msg.append(std::to_string(pid) + ", " + comm + ", ");
        msg.append(exe_path + ", ");
        msg.append(cmdline + ")");

        std::cout << msg << std::endl;
      }
    }
  });

  nl->new_fork.connect([&](int tgid, int child_pid, const std::string& child_comm) {
    if (child_comm == "wineserver") {
      auto games = cfg->get_games();

      if (games.find("wineserver") != games.end()) {
        std::cout << "wineserver pid: " + std::to_string(child_pid) << std::endl;

        tweaks->apply_process("wineserver", child_pid);
      }
    } else {
      if (tgid == game_pid && child_pid != tgid) {
        tweaks->apply_process(game_name, child_pid);
      }
    }
  });

  nl->new_exit.connect([&](int pid) {
    if (pid == game_pid) {
      std::cout << "No games running." << std::endl;

      tweaks->remove();
    }
  });

  // This is a blocking call. It has to be started at the end

  if (nl->listen) {
    nl->handle_events();
  }

  return 0;
}
