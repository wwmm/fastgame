#pragma once

#include <NVCtrl/NVCtrl.h>
#include <X11/Xlib.h>
#include <algorithm>
#include <iostream>
#include <memory>
#include "NVCtrl/NVCtrlLib.h"
#include "nvml.hpp"
#include "util.hpp"

namespace nvidia_wrapper {

class Nvidia {
 public:
  Nvidia();

  std::unique_ptr<Nvml> nvml;

  void set_clock_offset(const int& gpu_index, const int& gpu_offset, const int& memory_offset);
  void set_powermizer_mode(const int& gpu_index, const int& mode_id);
  bool has_gpu();

  auto get_gpu_clock_offset_range() -> std::pair<int, int>;

  auto get_memory_clock_offset_range() -> std::pair<int, int>;

 private:
  bool found_gpu = false;

  Display* dpy;
  int max_performance_mode = 3, min_gpu_clock_offset = 0, max_gpu_clock_offset = 0, min_memory_clock_offset = 0,
      max_memory_clock_offset = 0;

  void get_max_performance_mode(const int& gpu_index);
  void get_valid_clock_offset_values(const int& gpu_index);
};

}  // namespace nvidia_wrapper
