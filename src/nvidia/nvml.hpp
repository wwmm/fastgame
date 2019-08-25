#ifndef NVML_HPP
#define NVML_HPP

#include <iostream>

typedef void* nvmlDevice_t;

enum nvmlReturn_t {
  NVML_SUCCESS = 0,
  NVML_ERROR_UNINITIALIZED = 1,
  NVML_ERROR_INVALID_ARGUMENT = 2,
  NVML_ERROR_NOT_SUPPORTED = 3,
  NVML_ERROR_NO_PERMISSION = 4,
  NVML_ERROR_ALREADY_INITIALIZED = 5,
  NVML_ERROR_NOT_FOUND = 6,
  NVML_ERROR_INSUFFICIENT_SIZE = 7,
  NVML_ERROR_INSUFFICIENT_POWER = 8,
  NVML_ERROR_DRIVER_NOT_LOADED = 9,
  NVML_ERROR_TIMEOUT = 10,
  NVML_ERROR_IRQ_ISSUE = 11,
  NVML_ERROR_LIBRARY_NOT_FOUND = 12,
  NVML_ERROR_FUNCTION_NOT_FOUND = 13,
  NVML_ERROR_CORRUPTED_INFOROM = 14,
  NVML_ERROR_GPU_IS_LOST = 15,
  NVML_ERROR_RESET_REQUIRED = 16,
  NVML_ERROR_OPERATING_SYSTEM = 17,
  NVML_ERROR_LIB_RM_VERSION_MISMATCH = 18,
  NVML_ERROR_IN_USE = 19,
  NVML_ERROR_MEMORY = 20,
  NVML_ERROR_NO_DATA = 21,
  NVML_ERROR_VGPU_ECC_NOT_SUPPORTED = 22,
  NVML_ERROR_UNKNOWN = 999
};

class Nvml {
 public:
  Nvml();

  void set_power_limit(const int& gpu_index, const int& value);

 private:
  std::string log_tag = "nvml: ";

  void* handle;

  nvmlReturn_t (*nvmlInit)(void);
  nvmlReturn_t (*nvmlDeviceGetHandleByIndex)(uint, nvmlDevice_t*);
  nvmlReturn_t (*nvmlDeviceSetPowerManagementLimit)(nvmlDevice_t, uint);
};

#endif