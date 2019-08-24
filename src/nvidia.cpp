#include "nvidia.hpp"
#include "NVCtrl/NVCtrlLib.h"

Nvidia::Nvidia() {
  dpy = XOpenDisplay(nullptr);

  if (!dpy) {
    std::cout << log_tag + "failed to open disaplay" << std::endl;
  }

  int major, minor;

  if (!XNVCTRLQueryVersion(dpy, &major, &minor)) {
    std::cout << "The NV-CONTROL X extension does not exist " << std::endl;
  } else {
    std::cout << "Using NV-CONTROL extension " + std::to_string(major) + "." + std::to_string(minor) + " on display " +
                     XDisplayName(nullptr)
              << std::endl;
  }
}