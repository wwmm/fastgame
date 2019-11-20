#include "radeon.hpp"
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

Radeon::Radeon() {
  if (fs::is_directory("/sys/class/drm/card0")) {
    found_gpu = true;
  }
}

bool Radeon::has_gpu() {
  return found_gpu;
}

void Radeon::set_power_dpm_force_performance_level(const std::string& mode_name) {
  auto path = fs::path("/sys/class/drm/card0/device/power_dpm_force_performance_level");

  std::ofstream f;

  f.open(path);

  f << mode_name;

  f.close();

  std::cout << log_tag + "changed power_dpm_force_performance_level to: " << mode_name << std::endl;
}

void Radeon::set_power_cap(const int& value) {
  if (value == -1) {
    return;
  }

  auto path = fs::path("/sys/class/drm/card0/device/hwmon/hwmon1/power1_cap");

  if (!fs::is_regular_file(path)) {
    std::cout << log_tag + "device " + path.string() + " does not exist!" << std::endl;
    std::cout << log_tag + "could not change power cap! " << std::endl;
  } else {
    std::ofstream f;

    f.open(path);

    f << value;

    f.close();

    std::cout << log_tag + "changed power1_cap to: " << value << " Microwatts" << std::endl;
  }
}