#include "nvidia.hpp"
#include <algorithm>
#include "NVCtrl/NVCtrlLib.h"

Nvidia::Nvidia() {
  dpy = XOpenDisplay(nullptr);

  if (!dpy) {
    std::cout << log_tag + "failed to open disaplay" << std::endl;
  }

  int major, minor;

  if (!XNVCTRLQueryVersion(dpy, &major, &minor)) {
    std::cout << log_tag + "The NV-CONTROL X extension does not exist " << std::endl;

    found_gpu = false;
  } else {
    std::cout << log_tag + "Using NV-CONTROL extension " + std::to_string(major) + "." + std::to_string(minor) +
                     " on display " + XDisplayName(nullptr)
              << std::endl;

    found_gpu = true;

    get_max_performance_mode(0);
    get_valid_clock_offset_values(0);

    nvml = std::make_unique<Nvml>();
  }
}

bool Nvidia::has_gpu() {
  return found_gpu;
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
  int val = 0;

  if (gpu_offset >= min_gpu_clock_offset && gpu_offset <= max_gpu_clock_offset) {
    XNVCTRLQueryTargetAttribute(dpy, NV_CTRL_TARGET_TYPE_GPU, gpu_index, max_performance_mode,
                                NV_CTRL_GPU_NVCLOCK_OFFSET, &val);

    if (val != gpu_offset) {
      bool s = XNVCTRLSetTargetAttributeAndGetStatus(dpy, NV_CTRL_TARGET_TYPE_GPU, gpu_index, max_performance_mode,
                                                     NV_CTRL_GPU_NVCLOCK_OFFSET, gpu_offset);

      if (s) {
        std::cout << log_tag + "setting a gpu overclock offset of " << gpu_offset << std::endl;
      } else {
        std::cout << log_tag + "failed to set a gpu overclock offset of " << gpu_offset << std::endl;
      }
    }
  }

  val = 0;

  if (memory_offset >= min_memory_clock_offset && memory_offset <= max_memory_clock_offset) {
    XNVCTRLQueryTargetAttribute(dpy, NV_CTRL_TARGET_TYPE_GPU, gpu_index, max_performance_mode,
                                NV_CTRL_GPU_MEM_TRANSFER_RATE_OFFSET, &val);

    if (val != memory_offset) {
      bool s = XNVCTRLSetTargetAttributeAndGetStatus(dpy, NV_CTRL_TARGET_TYPE_GPU, gpu_index, max_performance_mode,
                                                     NV_CTRL_GPU_MEM_TRANSFER_RATE_OFFSET, memory_offset);
      if (s) {
        std::cout << log_tag + "setting a memory overclock offset of " << memory_offset << std::endl;
      } else {
        std::cout << log_tag + "failed to set a memory overclock offset of " << memory_offset << std::endl;
      }
    }
  }
}

void Nvidia::set_powermizer_mode(const int& gpu_index, const std::string& mode_name) {
  int mode;

  if (mode_name == "maximum-performance") {
    mode = NV_CTRL_GPU_POWER_MIZER_MODE_PREFER_MAXIMUM_PERFORMANCE;
  } else if (mode_name == "adaptive") {
    mode = NV_CTRL_GPU_POWER_MIZER_MODE_ADAPTIVE;
  } else if (mode_name == "auto") {
    mode = NV_CTRL_GPU_POWER_MIZER_MODE_AUTO;
  } else {
    mode = NV_CTRL_GPU_POWER_MIZER_MODE_AUTO;
  }

  bool s = XNVCTRLSetTargetAttributeAndGetStatus(dpy, NV_CTRL_TARGET_TYPE_GPU, gpu_index, max_performance_mode,
                                                 NV_CTRL_GPU_POWER_MIZER_MODE, mode);

  if (s) {
    std::cout << log_tag + "setting powermizer to mode " << mode_name << std::endl;
  } else {
    std::cout << log_tag + "failed to set powermizer to mode " << mode_name << std::endl;
  }
}