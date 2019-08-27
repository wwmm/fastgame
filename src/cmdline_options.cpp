#include "cmdline_options.hpp"
#include "config.h"

namespace po = boost::program_options;

CmdlineOptions::CmdlineOptions(int argc, char* argv[]) : desc("Allowed options") {
  try {
    desc.add_options()("help", "Print this help message")("config,c", po::value<std::string>()->default_value(""),
                                                          "Configuration File Path")(
        "game,g", po::value<std::string>()->default_value(""), "Game name as written in our config file")(
        "run", po::value<std::string>()->default_value(""), "Execute the executable given as argument");

    po::store(po::command_line_parser(argc, argv).options(desc).allow_unregistered().run(), vm);

    po::notify(vm);

    if (vm.count("help")) {
      std::cout << desc << std::endl;
      exit(1);
    }

    config_file_path = vm["config"].as<std::string>();
    game = vm["game"].as<std::string>();
    game_exe = vm["run"].as<std::string>();
  } catch (const boost::program_options::error& ex) {
    std::cerr << ex.what() << '\n';
  }

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