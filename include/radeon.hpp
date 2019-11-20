#ifndef RADEON_HPP
#define RADEON_HPP

#include <iostream>

class Radeon {
 public:
  Radeon();

  bool has_gpu();
  void set_power_dpm_force_performance_level(const std::string& mode_name);
  void set_power_cap(const int& value);

 private:
  std::string log_tag = "radeon: ";
  bool found_gpu = false;
};

#endif