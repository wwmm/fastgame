#ifndef NVIDIA_HPP
#define NVIDIA_HPP

#include <NVCtrl/NVCtrl.h>
#include <X11/Xlib.h>
#include <iostream>

class Nvidia {
 public:
  Nvidia();

  void set_clock_offset(const int& gpu_index, const int& gpu_offset, const int& memory_offset);

 private:
  std::string log_tag = "nvidia: ";

  Display* dpy;
  int screen, max_performance_mode = 3;

  void get_max_performance_mode(const int& gpu_index);
};

#endif