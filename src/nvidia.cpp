#include "nvidia.hpp"
#include "NVCtrl/NVCtrlLib.h"

Nvidia::Nvidia() {
  dpy = XOpenDisplay(nullptr);

  if (!dpy) {
    std::cout << log_tag + "failed to open disaplay" << std::endl;
  }

  screen = DefaultScreen(dpy);

  if (!XNVCTRLIsNvScreen(dpy, screen)) {
    std::cout << "The default screen is not a nvidia screen. Searching another." << std::endl;

    for (int n = 0; n < ScreenCount(dpy); n++) {
      if (XNVCTRLIsNvScreen(dpy, n)) {
        screen = n;

        break;
      }
    }
  }

  std::cout << "Using screen " + std::to_string(screen) << std::endl;

  int major, minor;

  if (!XNVCTRLQueryVersion(dpy, &major, &minor)) {
    std::cout << "The NV-CONTROL X extension does not exist " << std::endl;
  } else {
    std::cout << "Using NV-CONTROL extension " + std::to_string(major) + "." + std::to_string(minor) + " on display " +
                     XDisplayName(nullptr)
              << std::endl;
  }
}

void Nvidia::set_offeset(const int& core, const int& memory) {
  // XNVCTRLSetStringAttribute
}