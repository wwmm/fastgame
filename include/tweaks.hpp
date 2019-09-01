#ifndef TWEAKS_HPP
#define TWEAKS_HPP

#include <filesystem>
#include <iostream>
#include <memory>
#include "config.h"
#include "config.hpp"
#include "scheduler.hpp"

#ifdef USE_NVIDIA
#include "nvidia/nvidia.hpp"
#endif

class Tweaks {
 public:
  Tweaks(Config* config);

  void apply_global();
  void apply_process(const std::string& game, const int& pid);
  void remove();

  template <typename T>
  void change_disk_parameter(const std::string& name, const T& value) {
    auto device = cfg->get_key<std::string>("general.disk.device", "");

    if (device != "") {
      if (std::filesystem::exists("/dev/" + device)) {
        auto f_path = "/sys/block/" + device + "/queue/" + name;
        std::ofstream f;

        f.open(f_path);

        f << value;

        f.close();

        std::cout << log_tag + "changed /dev/" + device + " " + name + " to: " << value << std::endl;
      }
    }
  }

 private:
  std::string log_tag = "tweaks: ";

  Config* cfg;
  std::unique_ptr<Scheduler> scheduler;

#ifdef USE_NVIDIA
  std::unique_ptr<Nvidia> nvidia;
#endif

  void change_cpu_governor(const std::string& name);
  void change_cfs_parameter(const std::string& name, const int& value);
  void change_iopriority(const std::string& game, const int& pid);
  void change_niceness(const std::string& game, const int& pid);
  void change_scheduler_policy(const std::string& game, const int& pid);
};

#endif