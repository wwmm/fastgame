#ifndef NVIDIA_HPP
#define NVIDIA_HPP

#include <NVCtrl/NVCtrl.h>
#include <X11/Xlib.h>
#include <iostream>

class Nvidia {
 public:
  Nvidia();

 private:
  std::string log_tag = "nvidia: ";

  Display* dpy;
};

#endif