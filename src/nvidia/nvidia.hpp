#ifndef NVIDIA_HPP
#define NVIDIA_HPP

#include <NVCtrl/NVCtrl.h>
#include <X11/Xlib.h>
#include <iostream>
#include <memory>
#include "nvml.hpp"

class Nvidia {
 public:
  Nvidia();

  std::unique_ptr<Nvml> nvml;

  void set_clock_offset(const int& gpu_index, const int& gpu_offset, const int& memory_offset);
  void set_powermizer_mode(const int& gpu_index, const std::string& mode_name);
  bool has_gpu();

 private:
  std::string log_tag = "nvidia: ";
  bool found_gpu = false;

  Display* dpy;
  int max_performance_mode = 3, min_gpu_clock_offset, max_gpu_clock_offset, min_memory_clock_offset,
      max_memory_clock_offset;

  void get_max_performance_mode(const int& gpu_index);
  void get_valid_clock_offset_values(const int& gpu_index);
};

#endif