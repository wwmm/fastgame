#include "cmdline_options.hpp"
#include "config.h"

CmdlineOptions::CmdlineOptions(int argc, char* argv[]) : desc("Allowed options") {
  desc.add_options()("help", "Print this help message")(
      "config", boost::program_options::value<std::string>()->default_value(""), "Configuration File Path")(
      "game", boost::program_options::value<std::string>()->default_value(""),
      "Game name as written in our config file")("run", boost::program_options::value<std::string>()->default_value(""),
                                                 "Execute game exe given as argument");

  boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);

  boost::program_options::notify(vm);

  if (vm.count("help")) {
    std::cout << desc << std::endl;
    exit(1);
  }

  config_file_path = vm["config"].as<std::string>();
  game = vm["game"].as<std::string>();
  game_exe = vm["run"].as<std::string>();

  if (config_file_path.empty()) {
    config_file_path = std::string(CONFIGDIR) + "/config.json";
  }
}

CmdlineOptions::~CmdlineOptions() {}

std::string CmdlineOptions::get_config_file_path() {
  return config_file_path;
}

std::string CmdlineOptions::get_game() {
  return game;
}

std::string CmdlineOptions::get_game_exe() {
  return game_exe;
}