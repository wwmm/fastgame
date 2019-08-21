#include "config.hpp"
#include <iostream>

Config::Config(const std::string& cfg_path) {
  boost::property_tree::read_json(cfg_path, root);
}
