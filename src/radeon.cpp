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