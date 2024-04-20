#include "nvidia.hpp"
// clang-format off
#include <X11/Xlib.h>
#include <NVCtrl/NVCtrl.h>
#include <NVCtrl/NVCtrlLib.h>
// clang-format on
#include <algorithm>
#include <memory>
#include <string>
#include <utility>
#include "nvml.hpp"
#include "util.hpp"

namespace nvidia_wrapper {

Display* dpy;

Nvidia::Nvidia() {
  dpy = XOpenDisplay(nullptr);

  if (!dpy) {
    util::warning("failed to open display");
  }

  int major = 0;
  int minor = 0;

  if (!XNVCTRLQueryVersion(dpy, &major, &minor)) {
    util::warning("The NV-CONTROL X extension does not exist ");

    found_gpu = false;
  } else {
    util::debug("Using NV-CONTROL extension " + util::to_string(major) + "." + util::to_string(minor) + " on display " +
                XDisplayName(nullptr));

    found_gpu = true;

    get_max_performance_mode(0);
    get_valid_clock_offset_values(0);

    nvml = std::make_unique<Nvml>();
  }
}

auto Nvidia::has_gpu() const -> bool {
  return found_gpu;
}

void Nvidia::get_max_performance_mode(const int& gpu_index) {
  char* result = nullptr;

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
    util::warning("Unable to query the valid values for NV_CTRL_GPU_NVCLOCK_OFFSET");
  } else {
    min_gpu_clock_offset = valid_values.u.range.min;
    max_gpu_clock_offset = valid_values.u.range.max;

    util::debug("[min, max] values for NV_CTRL_GPU_NVCLOCK_OFFSET: [" + util::to_string(min_gpu_clock_offset) + ", " +
                util::to_string(max_gpu_clock_offset) + "]");
  }

  ret = XNVCTRLQueryValidTargetAttributeValues(dpy, NV_CTRL_TARGET_TYPE_GPU, gpu_index,
                                               max_performance_mode,  // performance level
                                               NV_CTRL_GPU_MEM_TRANSFER_RATE_OFFSET, &valid_values);
  if (!ret) {
    util::warning("Unable to query the valid values for NV_CTRL_GPU_MEM_TRANSFER_RATE_OFFSET");
  } else {
    min_memory_clock_offset = valid_values.u.range.min;
    max_memory_clock_offset = valid_values.u.range.max;

    util::debug("[min, max] values for NV_CTRL_GPU_MEM_TRANSFER_RATE_OFFSET: [" +
                util::to_string(min_memory_clock_offset) + ", " + util::to_string(max_memory_clock_offset) + "]");
  }
}

void Nvidia::set_clock_offset(const int& gpu_index, const int& gpu_offset, const int& memory_offset) const {
  int val = 0;

  if (gpu_offset >= min_gpu_clock_offset && gpu_offset <= max_gpu_clock_offset) {
    XNVCTRLQueryTargetAttribute(dpy, NV_CTRL_TARGET_TYPE_GPU, gpu_index, max_performance_mode,
                                NV_CTRL_GPU_NVCLOCK_OFFSET, &val);

    if (val != gpu_offset) {
      bool s = XNVCTRLSetTargetAttributeAndGetStatus(dpy, NV_CTRL_TARGET_TYPE_GPU, gpu_index, max_performance_mode,
                                                     NV_CTRL_GPU_NVCLOCK_OFFSET, gpu_offset);

      if (s) {
        util::debug("setting a gpu overclock offset of " + util::to_string(gpu_offset));
      } else {
        util::warning("failed to set a gpu overclock offset of " + util::to_string(gpu_offset));
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
        util::debug("setting a memory overclock offset of " + util::to_string(memory_offset));
      } else {
        util::warning("failed to set a memory overclock offset of " + util::to_string(memory_offset));
      }
    }
  }
}

void Nvidia::set_powermizer_mode(const int& gpu_index, const int& mode_id) const {
  int mode = 0;
  std::string mode_name;

  switch (mode_id) {
    case 0: {
      mode = NV_CTRL_GPU_POWER_MIZER_MODE_AUTO;

      mode_name = "auto";

      break;
    }
    case 1: {
      mode = NV_CTRL_GPU_POWER_MIZER_MODE_ADAPTIVE;

      mode_name = "adaptive";

      break;
    }
    case 2: {
      mode = NV_CTRL_GPU_POWER_MIZER_MODE_PREFER_MAXIMUM_PERFORMANCE;

      mode_name = "maximum performance";

      break;
    }
    default: {
      mode = NV_CTRL_GPU_POWER_MIZER_MODE_AUTO;

      mode_name = "auto";

      break;
    }
  }

  bool s = XNVCTRLSetTargetAttributeAndGetStatus(dpy, NV_CTRL_TARGET_TYPE_GPU, gpu_index, max_performance_mode,
                                                 NV_CTRL_GPU_POWER_MIZER_MODE, mode);

  if (s) {
    util::debug("setting powermizer to mode " + mode_name);
  } else {
    util::warning("failed to set powermizer to mode " + mode_name);
  }
}

auto Nvidia::get_gpu_clock_offset_range() -> std::pair<int, int> {
  return {min_gpu_clock_offset, max_gpu_clock_offset};
}

auto Nvidia::get_memory_clock_offset_range() -> std::pair<int, int> {
  return {min_memory_clock_offset, max_memory_clock_offset};
}

}  // namespace nvidia_wrapper