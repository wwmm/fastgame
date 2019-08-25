#include "nvidia.hpp"
#include <algorithm>
#include "NVCtrl/NVCtrlLib.h"

Nvidia::Nvidia() {
  dpy = XOpenDisplay(nullptr);

  if (!dpy) {
    std::cout << log_tag + "failed to open disaplay" << std::endl;
  }

  screen = DefaultScreen(dpy);

  if (!XNVCTRLIsNvScreen(dpy, screen)) {
    std::cout << log_tag + "The default screen is not a nvidia screen. Searching another." << std::endl;

    for (int n = 0; n < ScreenCount(dpy); n++) {
      if (XNVCTRLIsNvScreen(dpy, n)) {
        screen = n;

        break;
      }
    }
  }

  std::cout << log_tag + "Using screen " + std::to_string(screen) << std::endl;

  int major, minor;

  if (!XNVCTRLQueryVersion(dpy, &major, &minor)) {
    std::cout << log_tag + "The NV-CONTROL X extension does not exist " << std::endl;
  } else {
    std::cout << log_tag + "Using NV-CONTROL extension " + std::to_string(major) + "." + std::to_string(minor) +
                     " on display " + XDisplayName(nullptr)
              << std::endl;
  }

  get_max_performance_mode(0);
  get_valid_clock_offset_values(0);

  // set_clock_offset(0, 100, 100);
}

void Nvidia::get_max_performance_mode(const int& gpu_index) {
  char* result;

  bool r = XNVCTRLQueryTargetStringAttribute(dpy, NV_CTRL_TARGET_TYPE_GPU, gpu_index, 0,
                                             NV_CTRL_STRING_PERFORMANCE_MODES, &result);

  if (r) {
    auto s = std::string(result);

    max_performance_mode = std::count(s.begin(), s.end(), ';');
  }
}

void Nvidia::get_valid_clock_offset_values(const int& gpu_index) {
  NVCTRLAttributeValidValuesRec valid_values;

  int ret = XNVCTRLQueryValidTargetAttributeValues(dpy, NV_CTRL_TARGET_TYPE_GPU, gpu_index,
                                                   max_performance_mode,  // performance level
                                                   NV_CTRL_GPU_NVCLOCK_OFFSET, &valid_values);
  if (!ret) {
    std::cout << log_tag + "Unable to query the valid values for NV_CTRL_GPU_NVCLOCK_OFFSET" << std::endl;
  } else {
    min_gpu_clock_offset = valid_values.u.range.min;
    max_gpu_clock_offset = valid_values.u.range.max;

    std::cout << log_tag + "[min, max] values for NV_CTRL_GPU_NVCLOCK_OFFSET: [" << min_gpu_clock_offset << ", "
              << max_gpu_clock_offset << "]" << std::endl;
  }

  ret = XNVCTRLQueryValidTargetAttributeValues(dpy, NV_CTRL_TARGET_TYPE_GPU, gpu_index,
                                               max_performance_mode,  // performance level
                                               NV_CTRL_GPU_MEM_TRANSFER_RATE_OFFSET, &valid_values);
  if (!ret) {
    std::cout << log_tag + "Unable to query the valid values for NV_CTRL_GPU_MEM_TRANSFER_RATE_OFFSET" << std::endl;
  } else {
    min_memory_clock_offset = valid_values.u.range.min;
    max_memory_clock_offset = valid_values.u.range.max;

    std::cout << log_tag + "[min, max] values for NV_CTRL_GPU_MEM_TRANSFER_RATE_OFFSET: [" << min_memory_clock_offset
              << ", " << max_memory_clock_offset << "]" << std::endl;
  }
}

void Nvidia::set_clock_offset(const int& gpu_index, const int& gpu_offset, const int& memory_offset) {
  if (gpu_offset >= min_gpu_clock_offset && gpu_offset <= max_gpu_clock_offset) {
    XNVCTRLSetTargetAttribute(dpy, NV_CTRL_TARGET_TYPE_GPU, gpu_index, max_performance_mode, NV_CTRL_GPU_NVCLOCK_OFFSET,
                              gpu_offset);
  }

  if (memory_offset >= min_memory_clock_offset && memory_offset <= max_memory_clock_offset) {
    XNVCTRLSetTargetAttribute(dpy, NV_CTRL_TARGET_TYPE_GPU, gpu_index, max_performance_mode,
                              NV_CTRL_GPU_MEM_TRANSFER_RATE_OFFSET, memory_offset);
  }
}