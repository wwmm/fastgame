#include "tweaks.hpp"
#include <sys/resource.h>
#include <filesystem>
#include "ioprio.hpp"

namespace fs = std::filesystem;

Tweaks::Tweaks(Config* config)
    : cfg(config), scheduler(std::make_unique<Scheduler>()), nvidia(std::make_unique<Nvidia>()) {}

void Tweaks::apply(const std::string& game, const int& pid) {
  auto governor = cfg->get_key<std::string>("general.cpu.game-governor", "performance");
  auto disk_scheduler = cfg->get_key<std::string>("general.disk.game-scheduler", "");
  auto disk_read_ahead = cfg->get_key("general.disk.game-read-ahead", 256);

  change_cpu_governor(governor);
  change_disk_scheduler(disk_scheduler);
  change_disk_read_ahead(disk_read_ahead);
  change_niceness(game, pid);
  change_cpu_scheduler_affinity_and_policy(game, pid);
  change_iopriority(game, pid);
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
    }
  }
}

void Tweaks::change_disk_read_ahead(const int& value) {
  auto device = cfg->get_key<std::string>("general.disk.device", "");

  if (device != "") {
    if (fs::exists("/dev/" + device)) {
      auto f_path = "/sys/block/" + device + "/queue/read_ahead_kb";
      std::ofstream f;

      f.open(f_path);

      f << value;

      f.close();
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

void Tweaks::remove() {
  auto governor = cfg->get_key<std::string>("general.cpu.default-governor", "schedutil");
  auto disk_scheduler = cfg->get_key<std::string>("general.disk.default-scheduler", "");
  auto disk_read_ahead = cfg->get_key("general.disk.default-read-ahead", 256);

  change_cpu_governor(governor);
  change_disk_scheduler(disk_scheduler);
  change_disk_read_ahead(disk_read_ahead);
}