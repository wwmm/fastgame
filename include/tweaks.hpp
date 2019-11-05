#ifndef TWEAKS_HPP
#define TWEAKS_HPP

#include <filesystem>
#include <iostream>
#include <memory>
#include "config.h"
#include "config.hpp"
#include "radeon.hpp"
#include "scheduler.hpp"

#ifdef USE_NVIDIA
#include "nvidia/nvidia.hpp"
#endif

class Tweaks {
 public:
  Tweaks(Config* config);

  int parent_thread_pid = -1;

  void apply_global();
  void apply_process(const std::string& game, const int& pid, const std::string& thread_name);
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
  std::unique_ptr<Radeon> radeon;

#ifdef USE_NVIDIA
  std::unique_ptr<Nvidia> nvidia;
#endif

  std::ofstream cpu_dma_ofstream;

  void change_cpu_governor(const std::string& name);
  void change_cfs_parameter(const std::string& name, const int& value);
  void change_iopriority(const std::string& game, const int& pid, const std::string& thread_name);
  void change_niceness(const std::string& game, const int& pid, const std::string& thread_name);
  void change_scheduler_policy(const std::string& game, const int& pid, const std::string& thread_name);
  void set_hugepages(const std::string& state, const std::string& defrag, const std::string& shmem_enabled);
  void set_cpu_dma_latency(const int& latency_us);
};

#endif