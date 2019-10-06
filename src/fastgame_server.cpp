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

  nl->new_exec.connect([&](int pid, std::string comm, std::string cmdline, std::string exe_path) {
    if (comm == "wineserver") {
      return;
    }

    for (auto game : cfg->get_games()) {
      auto apply = false;
      auto path_comm = fs::path(comm);

      if (game == comm || game == path_comm.stem().string()) {
        apply = true;
      } else if (comm.size() == 15) {  // comm is larger than 15 characters and was truncated by the kernel
        auto sub_str = game.substr(0, 15);

        if (comm == sub_str) {
          apply = true;
        }
      }

      if (apply) {
        auto thread_name = fs::path(comm).stem().string();

        tweaks->apply_process(game, pid, thread_name);

        if (pid_list.size() == 0) {
          tweaks->apply_global();
        }

        pid_list.push_back(std::pair(game, pid));

        std::cout << game + " parent thread name: " << thread_name << std::endl;

        std::cout << "(" + std::to_string(pid) + ", " + comm + ", " + exe_path + ", " + cmdline + ")" << std::endl;
      }
    }

    for (auto& p : pid_list) {
      if (pid != p.second) {
        try {
          auto task_dir = "/proc/" + std::to_string(p.second) + "/task";

          for (const auto& entry : fs::directory_iterator(task_dir)) {
            const auto task_pid = std::stoi(entry.path().filename().string());

            if (task_pid != p.second && task_pid == pid) {
              auto thread_name = fs::path(comm).stem().string();

              tweaks->apply_process(p.first, pid, thread_name);

              std::cout << "new " + p.first + " thread: (" + comm + ", " + std::to_string(pid) + ")" << std::endl;

              break;
            }
          }
        } catch (std::exception& e) {
        }
      }
    }
  });

  nl->new_fork.connect([&](int child_pid, std::string child_comm) {
    if (child_comm == "wineserver") {
      std::cout << "wineserver pid: " + std::to_string(child_pid) << std::endl;

      tweaks->apply_process("wineserver", child_pid, "wineserver");
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
