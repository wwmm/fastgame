#ifndef Cmdline_Options_HPP
#define Cmdline_Options_HPP

#include <iostream>
#include "boost/program_options.hpp"

class CmdlineOptions {
 public:
  CmdlineOptions(int argc, char* argv[]);

  virtual ~CmdlineOptions();

  std::string get_config_file_path();

  std::string get_game();

  std::string get_game_exe();

 private:
  std::string log_tag = "cmdline_options: ";

  std::string config_file_path, game, game_exe;

  boost::program_options::options_description desc;

  boost::program_options::variables_map vm;
};

#endif
