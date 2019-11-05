#include "config.hpp"
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

Config::Config(const std::string& cfg_path) {
  boost::property_tree::read_json(cfg_path, root);

  profiles_dir = fs::path(cfg_path).parent_path().append("profiles").string();

  std::cout << log_tag + "profiles directory: " + profiles_dir << std::endl;

  for (const auto& entry : fs::directory_iterator(profiles_dir)) {
    if (entry.path().extension().string() == ".json") {
      auto filename = entry.path().filename().string();

      std::cout << log_tag + "found profile: " + filename << std::endl;

      boost::property_tree::ptree r;

      boost::property_tree::read_json(profiles_dir + "/" + filename, r);

      games.insert(std::pair(r.get<std::string>("executable-name", ""), filename));
    }
  }

  std::cout << log_tag + "profiles directory has " + std::to_string(games.size()) + " profiles" << std::endl;
}

std::map<std::string, std::string> Config::get_games() {
  return games;
}

std::string Config::get_profiles_dir() {
  return profiles_dir;
}
