#include "tweaks.hpp"
#include <sys/resource.h>
#include <filesystem>
#include "ioprio.hpp"

namespace fs = std::filesystem;

Tweaks::Tweaks(Config* config) : cfg(config), scheduler(std::make_unique<Scheduler>()) {
#ifdef USE_NVIDIA
  nvidia = std::make_unique<Nvidia>();
#endif
}

void Tweaks::apply_global() {
  auto governor = cfg->get_key<std::string>("general.cpu.game-governor", "performance");
  auto disk_scheduler = cfg->get_key<std::string>("general.disk.game-scheduler", "");
  auto disk_read_ahead = cfg->get_key("general.disk.game-read-ahead", -1);
  auto disk_nr_requests = cfg->get_key("general.disk.game-nr-requests", -1);
  auto disk_rq_affinity = cfg->get_key("general.disk.game-rq-affinity", -1);

  change_cpu_governor(governor);
  change_disk_scheduler(disk_scheduler);
  change_disk_read_ahead(disk_read_ahead);
  change_disk_nr_requests(disk_nr_requests);
  change_disk_rq_affinity(disk_rq_affinity);

#ifdef USE_NVIDIA
  auto gpu_offset = cfg->get_key("general.nvidia.game-gpu-clock-offset", 0);
  auto memory_offset = cfg->get_key("general.nvidia.game-memory-clock-offset", 0);
  auto powermizer_mode = cfg->get_key<std::string>("general.nvidia.game-powermizer-mode", "auto");
  auto power_limit = cfg->get_key("general.nvidia.game-power-limit", -1);

  nvidia->set_powermizer_mode(0, powermizer_mode);
  nvidia->set_clock_offset(0, gpu_offset, memory_offset);
  nvidia->nvml->set_power_limit(0, power_limit);
#endif
}

void Tweaks::apply_process(const std::string& game, const int& pid) {
  change_niceness(game, pid);
  change_cpu_scheduler_affinity_and_policy(game, pid);
  change_iopriority(game, pid);
}

void Tweaks::remove() {
  auto governor = cfg->get_key<std::string>("general.cpu.default-governor", "schedutil");
  auto disk_scheduler = cfg->get_key<std::string>("general.disk.default-scheduler", "");
  auto disk_read_ahead = cfg->get_key("general.disk.default-read-ahead", -1);
  auto disk_nr_requests = cfg->get_key("general.disk.default-nr-requests", -1);
  auto disk_rq_affinity = cfg->get_key("general.disk.default-rq-affinity", -1);

  change_cpu_governor(governor);
  change_disk_scheduler(disk_scheduler);
  change_disk_read_ahead(disk_read_ahead);
  change_disk_nr_requests(disk_nr_requests);
  change_disk_rq_affinity(disk_rq_affinity);

#ifdef USE_NVIDIA
  auto gpu_offset = cfg->get_key("general.nvidia.default-gpu-clock-offset", 0);
  auto memory_offset = cfg->get_key("general.nvidia.default-memory-clock-offset", 0);
  auto powermizer_mode = cfg->get_key<std::string>("general.nvidia.default-powermizer-mode", "auto");
  auto power_limit = cfg->get_key("general.nvidia.default-power-limit", -1);

  nvidia->set_powermizer_mode(0, powermizer_mode);
  nvidia->set_clock_offset(0, gpu_offset, memory_offset);
  nvidia->nvml->set_power_limit(0, power_limit);
#endif
}

void Tweaks::change_cpu_governor(const std::string& name) {
  auto path = fs::path("/sys/devices/system/cpu/present");

  std::ostringstream stream;

  stream << std::ifstream(path).rdbuf();

  auto out = stream.str();

  out.erase(0, 2);  // remove the characters 0-

  int ncores = std::stoi(out);

  for (int n = 0; n < ncores; n++) {
    std::ofstream f;
    auto f_path = "/sys/devices/system/cpu/cpu" + std::to_string(n) + "/cpufreq/scaling_governor";

    f.open(f_path);

    f << name;

    f.close();
  }

  std::cout << log_tag + "changed cpu frequency governor to: " << name << std::endl;
}

void Tweaks::change_disk_scheduler(const std::string& name) {
  auto device = cfg->get_key<std::string>("general.disk.device", "");

  if (device != "") {
    if (fs::exists("/dev/" + device)) {
      auto f_path = "/sys/block/" + device + "/queue/scheduler";
      std::ofstream f;

      f.open(f_path);

      f << name;

      f.close();

      std::cout << log_tag + "changed /dev/" + device + " scheduler to: " << name << std::endl;
    }
  }
}

void Tweaks::change_disk_read_ahead(const int& value) {
  if (value == -1) {
    return;
  }

  auto device = cfg->get_key<std::string>("general.disk.device", "");

  if (device != "") {
    if (fs::exists("/dev/" + device)) {
      auto f_path = "/sys/block/" + device + "/queue/read_ahead_kb";
      std::ofstream f;

      f.open(f_path);

      f << value;

      f.close();

      std::cout << log_tag + "changed /dev/" + device + " read_ahead value to: " << value << std::endl;
    }
  }
}

void Tweaks::change_disk_nr_requests(const int& value) {
  if (value == -1) {
    return;
  }

  auto device = cfg->get_key<std::string>("general.disk.device", "");

  if (device != "") {
    if (fs::exists("/dev/" + device)) {
      auto f_path = "/sys/block/" + device + "/queue/nr_requests";
      std::ofstream f;

      f.open(f_path);

      f << value;

      f.close();

      std::cout << log_tag + "changed /dev/" + device + " nr_requests value to: " << value << std::endl;
    }
  }
}

void Tweaks::change_disk_rq_affinity(const int& value) {
  if (value == -1) {
    return;
  }

  auto device = cfg->get_key<std::string>("general.disk.device", "");

  if (device != "") {
    if (fs::exists("/dev/" + device)) {
      auto f_path = "/sys/block/" + device + "/queue/rq_affinity";
      std::ofstream f;

      f.open(f_path);

      f << value;

      f.close();

      std::cout << log_tag + "changed /dev/" + device + " rq_affinity value to: " << value << std::endl;
    }
  }
}

void Tweaks::change_niceness(const std::string& game, const int& pid) {
  auto niceness = cfg->get_key("games." + game + ".niceness", 0);

  if (setpriority(PRIO_PROCESS, pid, niceness) < 0) {
    // std::cout << log_tag + "could not set process " + std::to_string(pid) + " niceness" << std::endl;
  }
}

void Tweaks::change_cpu_scheduler_affinity_and_policy(const std::string& game, const int& pid) {
  auto affinity_cores = cfg->get_key_array<int>("games." + game + ".affinity-cores");
  auto sched_policy = cfg->get_key<std::string>("games." + game + ".scheduler-policy", "SCHED_OTHER");
  auto sched_priority = cfg->get_key("games." + game + ".scheduler-policy-priority", 0);

  scheduler->set_affinity(pid, affinity_cores);
  scheduler->set_policy(pid, sched_policy, sched_priority);
}

void Tweaks::change_iopriority(const std::string& game, const int& pid) {
  auto io_class = cfg->get_key<std::string>("games." + game + ".io-class", "BE");
  auto io_priority = cfg->get_key("games." + game + ".io-priority", 7);

  if (ioprio_set(pid, io_class, io_priority) != 0) {
    // std::cout << log_tag + "could not set process " + std::to_string(pid) + " io class and priority" << std::endl;
  }
}
