#include "config.hpp"
#include <iostream>

Config::Config(const std::string& cfg_path) {
  boost::property_tree::read_json(cfg_path, root);

  auto games_section = root.get_child("games");

  std::cout << "games process names in our config:" << std::endl;

  for (auto& it : games_section) {
    std::cout << it.first << std::endl;

    games.push_back(it.first);
  }
}

std::vector<std::string> Config::get_games() {
  return games;
}
