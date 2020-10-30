#include "util.hpp"
#include <glib.h>
#include <algorithm>
#include <filesystem>
#include <fstream>

namespace util {

void debug(const std::string& s) {
  g_debug(s.c_str(), "%s");
}

void error(const std::string& s) {
  g_error(s.c_str(), "%s");
}

void critical(const std::string& s) {
  g_critical(s.c_str(), "%s");
}

void warning(const std::string& s) {
  g_warning(s.c_str(), "%s");
}

void info(const std::string& s) {
  g_info(s.c_str(), "%s");
}

auto read_system_file_options(const std::string& path_str) -> std::tuple<std::vector<std::string>, std::string> {
  std::vector<std::string> list;
  std::string current_value;

  auto path = std::filesystem::path(path_str);

  if (!std::filesystem::is_regular_file(path)) {
    util::debug("the file " + path.string() + " does not exist!");
  } else {
    std::ifstream f;

    f.open(path, std::ios::in);

    std::string value;

    while (std::getline(f, value, ' ')) {
      value.erase(std::remove(value.begin(), value.end(), '\n'), value.end());

      if (value.find('[') != std::string::npos) {
        current_value = value.erase(0, 1).erase(value.size() - 1, 1);  // removing the [] characters
      }

      list.emplace_back(value);
    }

    f.close();
  }

  return std::make_tuple(list, current_value);
}

}  // namespace util
