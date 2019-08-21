#include "cmdline_options.hpp"

CmdlineOptions::CmdlineOptions(int argc, char* argv[]) : desc("Allowed options") {
  desc.add_options()("help", "Print this help message")(
      "config", boost::program_options::value<std::string>()->default_value(""), "Configuration File Path");

  boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);

  boost::program_options::notify(vm);

  if (vm.count("help")) {
    std::cout << desc << std::endl;
    exit(1);
  }

  config_file_path = vm["config"].as<std::string>();

  if (config_file_path.empty()) {
    std::cout << "no config file!" << std::endl;
  }
}

CmdlineOptions::~CmdlineOptions() {}

std::string CmdlineOptions::get_config_file_path() {
  return config_file_path;
}