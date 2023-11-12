#include <sys/prctl.h>
#include <unistd.h>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <filesystem>
#include <iostream>
#include "util.hpp"

auto main(int argc, char* argv[]) -> int {
  auto input_file = std::filesystem::temp_directory_path() / std::filesystem::path{"fastgame.json"};

  if (!std::filesystem::is_regular_file(input_file)) {
    util::error("fastgame_launcher: can not read: " + input_file.string());
  }

  boost::property_tree::ptree root;

  boost::property_tree::read_json(input_file.string(), root);

  // environment variables

  try {
    for (const auto& c : root.get_child("environment-variables")) {
      std::string key_and_value = c.second.data();

      int delimiter_position = key_and_value.find('=');

      auto key = key_and_value.substr(0, delimiter_position);
      auto value = key_and_value.substr(delimiter_position + 1);

      setenv(key.c_str(), value.c_str(), 1);
    }
  } catch (const boost::property_tree::ptree_error& e) {
    util::warning("fastgame_launcher: error when parsing the environmental variables list");
  }

  // game's command line arguments

  std::vector<std::string> game_cmd_arguments;

  try {
    for (const auto& c : root.get_child("command-line-arguments")) {
      game_cmd_arguments.emplace_back(c.second.data());
    }
  } catch (const boost::property_tree::ptree_error& e) {
    util::warning("fastgame_launcher: error when parsing the game's command line arguments list");
  }

  // setting the initial cpu affinity and priority scheduler

  std::vector<int> cpu_affinity;

  try {
    for (const auto& c : root.get_child("cpu.game-cores")) {
      int core_index = std::stoi(c.second.data());

      cpu_affinity.emplace_back(core_index);
    }

  } catch (const boost::property_tree::ptree_error& e) {
    util::warning("fastgame_launcher: error when parsing the cpu core list");
  }

  util::apply_cpu_affinity(0, cpu_affinity);

  if (root.get<bool>("cpu.use-batch-scheduler")) {
    util::set_process_scheduler(0, SCHED_BATCH, 0);
  }

  int timer_slack_ns = root.get<int>("cpu.timer-slack", 50000);

  if (prctl(PR_SET_TIMERSLACK, timer_slack_ns, 0, 0, 0) != 0) {
    util::warning("fastgame_launcher: could not set the timer slack value");
  }

  // Assuming that the game executable is the first argument passed as option to fastgame_laucher

  std::vector<char*> arguments;

  if (argc > 1) {
    for (int n = 1; n < argc; n++) {
      // std::cout << argv[n] << std::endl;

      arguments.push_back(argv[n]);
    }

    for (auto& arg : game_cmd_arguments) {
      arguments.push_back(&arg.front());
    }

    arguments.push_back(nullptr);

    execvpe(argv[1], &arguments.front(), environ);

    return 0;
  }

  util::warning("fastgame_launcher: you have to pass the game executable as argument!");

  return -1;
}