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

  // process scheduler

  if (root.get<bool>("cpu.use-batch-scheduler")) {
    int policy_index = SCHED_BATCH;
    sched_param policy_params{};

    policy_params.sched_priority = 0;

    sched_setscheduler(0, policy_index, &policy_params);
  }

  // Assuming that the game executable is the only argument passed as option

  std::vector<char*> arguments;

  if (argc > 1) {
    for (int n = 1; n < argc; n++) {
      // std::cout << argv[n] << std::endl;

      arguments.push_back(argv[n]);
    }

    arguments.push_back(nullptr);

    execvpe(argv[1], &arguments.front(), environ);

    return 0;
  }

  util::warning("fastgame_launcher: you have to pass the game executable as argument!");

  return -1;
}