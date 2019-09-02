#include "cgroups.hpp"

#include <filesystem>

namespace fs = std::filesystem;

Cgroups::Cgroups(Config* config) : cfg(config) {
  if (!std::filesystem::exists(parent_dir)) {
    fs::create_directory(parent_dir);

    std::cout << log_tag + "created cgroup: " << parent_dir << std::endl;
  } else {
    std::cout << log_tag + "cgroup " + parent_dir + " already exists" << std::endl;
  }

  if (!std::filesystem::exists(children_dir)) {
    fs::create_directory(children_dir);

    std::cout << log_tag + "created cgroup: " << children_dir << std::endl;
  } else {
    std::cout << log_tag + "cgroup " + children_dir + " already exists" << std::endl;
  }
}

void Cgroups::set(const std::string& game, const int& pid, const bool& is_parent) {
  if (is_parent) {
    auto parent_shares = cfg->get_key("games." + game + ".cgroups.cpu-shares.parent", -1);

    if (parent_shares == -1) {
      return;
    }

    std::ofstream f;

    f.open(parent_dir + "/cpu.shares");

    f << parent_shares;

    f.close();
  } else {
    auto children_shares = cfg->get_key("games." + game + ".cgroups.cpu-shares.children", -1);

    if (children_shares == -1) {
      return;
    }

    std::ofstream f;

    f.open(children_dir + "/cpu.shares");

    f << children_shares;

    f.close();
  }
}