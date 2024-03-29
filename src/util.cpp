#include "util.hpp"

namespace util {

auto prepare_debug_message(const std::string& message, source_location location) -> std::string {
  auto file_path = std::filesystem::path{location.file_name()};

  std::string msg = "\t" + file_path.filename().string() + ":" + to_string(location.line()) + "\t" + message;

  return msg;
}

void debug(const std::string& s, source_location location) {
  g_debug(prepare_debug_message(s, location).c_str(), "%s");
}

void error(const std::string& s, source_location location) {
  g_error(prepare_debug_message(s, location).c_str(), "%s");
}

void critical(const std::string& s, source_location location) {
  g_critical(prepare_debug_message(s, location).c_str(), "%s");
}

void warning(const std::string& s, source_location location) {
  g_warning(prepare_debug_message(s, location).c_str(), "%s");
}

void info(const std::string& s, source_location location) {
  g_info(prepare_debug_message(s, location).c_str(), "%s");
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

  if (!std::filesystem::is_directory(path)) {
    util::debug("The directory " + path.string() + " does not exist!");

    return index;
  }

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

void clear_cpu_affinity(const int& pid) {
  uint n_cores = std::thread::hardware_concurrency();

  cpu_set_t mask;

  CPU_ZERO(&mask);  // Initialize it all to 0, i.e. no CPUs selected.

  for (uint n = 0; n < n_cores; n++) {
    CPU_SET(n, &mask);
  }

  sched_setaffinity(pid, sizeof(cpu_set_t), &mask);
}

void set_process_scheduler(const int& pid, const int& policy_index, const int& priority) {
  sched_param policy_params{};

  policy_params.sched_priority = priority;

  sched_setscheduler(pid, policy_index, &policy_params);
}

auto card_is_amdgpu(const int& card_index) -> bool {
  auto path =
      std::filesystem::path("/sys/class/drm/card" + std::to_string(card_index) + "/device/driver/module/drivers/");

  if (std::filesystem::is_directory(path)) {
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
      if (std::filesystem::is_directory(entry)) {
        if (std::filesystem::path(entry).filename().string().find("amdgpu") != std::string::npos) {
          return true;
        }
      }
    }
  }

  return false;
}

auto get_amdgpu_indices() -> std::vector<int> {
  std::vector<int> card_indices;

  auto dir_path = std::filesystem::path("/sys/class/drm/");

  if (std::filesystem::is_directory(dir_path)) {
    for (const auto& entry : std::filesystem::directory_iterator(dir_path)) {
      if (std::filesystem::is_directory(entry)) {
        auto folder_name = entry.path().filename().string();

        int idx = 0;

        if (folder_name.starts_with("card") &&
            folder_name.size() == 5) {  // name is "card" plus a number between 0 and 9

          idx = static_cast<int>(folder_name.back() - '0');

          if (util::card_is_amdgpu(idx)) {
            card_indices.emplace_back(idx);

            // util::warning(folder_name + " -> " + folder_name.back() + " -> " + util::to_string(idx));
          }
        }
      }
    }
  }

  return card_indices;
}

auto remove_filename_extension(const std::string& basename) -> std::string {
  return basename.substr(0, basename.find_last_of('.'));
}

auto sys_class_path_to_mounting_path(const std::string& sys_class_path) -> std::string {
  // Assuming /sys/class/block as root path
  auto dev_path = "/dev/" + sys_class_path.substr(17);

  std::ifstream infile("/proc/self/mounts");

  std::string psm_dev_path, mounting_point;

  while (infile >> psm_dev_path >> mounting_point) {
    if (psm_dev_path.starts_with(dev_path)) {
      return mounting_point;
    }
  }

  return "";
}

auto mounting_path_to_sys_class_path(const std::string& mounting_path) -> std::string {
  std::ifstream infile("/proc/self/mounts");

  std::string psm_dev_path, psm_mounting_point;

  while (infile >> psm_dev_path >> psm_mounting_point) {
    if (psm_mounting_point == mounting_path && psm_dev_path.starts_with("/dev/")) {
      auto psm_dev_name = psm_dev_path.substr(5);

      for (const auto& entry : std::filesystem::directory_iterator("/sys/class/block")) {
        auto path = entry.path().string();

        // this is the actual device and not a partition
        if (std::filesystem::is_regular_file(path + "/queue/scheduler")) {
          auto dev_name = path.substr(17);

          if (psm_dev_name.starts_with(dev_name)) {
            return path;
          }
        }
      }
    }
  }

  return "";
}

}  // namespace util
