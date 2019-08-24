#ifndef NVIDIA_HPP
#define NVIDIA_HPP

#include <NVCtrl/NVCtrl.h>
#include <X11/Xlib.h>
#include <iostream>

class Nvidia {
 public:
  Nvidia();

  void set_offeset(const int& core, const int& memory);

 private:
  std::string log_tag = "nvidia: ";

  Display* dpy;
  int screen;
};

#endif