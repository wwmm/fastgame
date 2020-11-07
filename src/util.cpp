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

auto read_system_setting(const std::string& path_str) -> std::vector<std::string> {
  std::vector<std::string> list;

  auto path = std::filesystem::path(path_str);

  if (!std::filesystem::is_regular_file(path)) {
    util::debug("the file " + path.string() + " does not exist!");
  } else {
    std::ifstream f;

    f.open(path);

    std::string value;

    while (std::getline(f, value, ' ')) {
      value.erase(std::remove(value.begin(), value.end(), '\n'), value.end());

      if (!value.empty()) {
        list.emplace_back(value);
      }
    }

    f.close();
  }

  return list;
}

auto get_selected_value(const std::vector<std::string>& list) -> std::string {
  std::string selected_value;

  for (auto value : list) {
    if (value.find('[') != std::string::npos) {
      selected_value = value.erase(0, 1).erase(value.size() - 1, 1);  // removing the [] characters
    }
  }

  return selected_value;
}

auto find_hwmon_index(const int& card_index) -> int {
  int index = 0;
  auto path = std::filesystem::path("/sys/class/drm/card" + std::to_string(card_index) + "/device/hwmon/");

  for (const auto& entry : std::filesystem::directory_iterator(path)) {
    auto child_directory = std::filesystem::path(entry).filename().string();

    // It is unlikely that a system has more than 10 hwmon devices so we just get the last character

    std::string index_str(1, child_directory.back());

    index = std::stoi(index_str);

    break;
  }

  return index;
}

void apply_cpu_affinity(const int& pid, const std::vector<int>& cpu_affinity) {
  cpu_set_t mask;

  CPU_ZERO(&mask);  // Initialize it all to 0, i.e. no CPUs selected.

  for (const auto& core_index : cpu_affinity) {
    CPU_SET(core_index, &mask);
  }

  if (sched_setaffinity(pid, sizeof(cpu_set_t), &mask) < 0) {
    util::warning("fastgame_apply: could not set cpu affinity for the process: " + std::to_string(pid));
  }
}

void set_process_scheduler(const int& pid, const int& policy_index, const int& priority) {
  sched_param policy_params{};

  policy_params.sched_priority = priority;

  sched_setscheduler(pid, policy_index, &policy_params);
}

}  // namespace util
