#include <unistd.h>
#include <boost/algorithm/string.hpp>
#include <memory>
#include "cmdline_options.hpp"
#include "config.h"
#include "config.hpp"
#include "scheduler.hpp"

int main(int argc, char* argv[]) {
  auto cmd_options = std::make_unique<CmdlineOptions>(argc, argv);

  auto cfg = std::make_unique<Config>(cmd_options->get_config_file_path());

  auto scheduler = std::make_unique<Scheduler>();

  auto game = cmd_options->get_game();
  auto game_exe = cmd_options->get_game_exe();
  auto environment = cfg->get_key_array<std::string>("games." + game + ".environment");

  scheduler->set_affinity(0, cfg->get_key_array<int>("games." + game + ".cpu-affinity"));

  for (auto& env : environment) {
    std::vector<std::string> results;

    boost::split(results, env, [](char c) { return c == '='; });

    if (results.size() == 2) {
      // std::cout << results[0] << "\t" << results[1] << std::endl;

      setenv(results[0].c_str(), results[1].c_str(), 1);
    }
  }

  char* args[] = {const_cast<char*>(game_exe.c_str()), 0};
  extern char** environ;

  execvpe(game_exe.c_str(), args, environ);

  return 0;
}