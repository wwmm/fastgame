#include "config.hpp"
#include <iostream>

Config::Config(const std::string& cfg_path) {
  boost::property_tree::read_json(cfg_path, root);

  for (auto& it : root) {
    games.push_back(it.first);
  }
}

std::vector<std::string> Config::get_games() {
  return games;
}
