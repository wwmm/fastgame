#include "nvml.hpp"
#include <dlfcn.h>
#include <sys/types.h>
#include <iostream>

Nvml::Nvml() {
  handle = dlopen("libnvidia-ml.so", RTLD_LAZY);

  if (!handle) {
    std::cout << log_tag + "error opening libnvidia-ml.so: " << dlerror() << '\n';
  } else {
    nvmlInit = (nvmlReturn_t(*)())dlsym(handle, "nvmlInit");
    nvmlDeviceGetHandleByIndex = (nvmlReturn_t(*)(uint, nvmlDevice_t*))dlsym(handle, "nvmlDeviceGetHandleByIndex");
    nvmlDeviceSetPowerManagementLimit =
        (nvmlReturn_t(*)(nvmlDevice_t, uint))dlsym(handle, "nvmlDeviceSetPowerManagementLimit");

    nvmlReturn_t r = nvmlInit();

    if (r != NVML_SUCCESS) {
      std::cout << log_tag + " nvmlInit error: " << r << '\n';
    }
  }
}

void Nvml::set_power_limit(const int& gpu_index, const int& value) {
  if (handle && value > 0) {
    nvmlDevice_t device = nullptr;

    nvmlReturn_t r = nvmlDeviceGetHandleByIndex(gpu_index, &device);

    if (r != NVML_SUCCESS) {
      std::cout << log_tag + " nvmlDeviceGetHandleByIndex error: " << r << '\n';
    }

    r = nvmlDeviceSetPowerManagementLimit(device, value * 1000);  // power input is in milliwatts

    if (r != NVML_SUCCESS) {
      std::cout << log_tag + " nvmlDeviceSetPowerManagementLimit error: " << r << '\n';
    } else {
      std::cout << log_tag + "setting power limit to: " << value << " W" << '\n';
    }
  }
}