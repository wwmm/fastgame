#ifndef Cmdline_Options_HPP
#define Cmdline_Options_HPP

#include <iostream>
#include "boost/program_options.hpp"

class CmdlineOptions {
 public:
  CmdlineOptions(int argc, char* argv[]);

  virtual ~CmdlineOptions();

  std::string get_config_file_path();

 private:
  std::string config_file_path;

  boost::program_options::options_description desc;

  boost::program_options::variables_map vm;
};

#endif
