#include "tweaks.hpp"
#include <sys/resource.h>
#include "ioprio.hpp"

namespace fs = std::filesystem;

Tweaks::Tweaks(Config* config) : cfg(config), scheduler(std::make_unique<Scheduler>()) {
#ifdef USE_NVIDIA
  nvidia = std::make_unique<Nvidia>();
#endif
}

void Tweaks::apply_global() {
  auto governor = cfg->get_key<std::string>("general.cpu.governor.game", "performance");
  auto sched_child_runs_first = cfg->get_key("general.cpu.scheduler.sched_child_runs_first.game", -1);

  auto hugepage_state = cfg->get_key<std::string>("general.memory.hugepage.game", "");
  auto hugepage_defrag = cfg->get_key<std::string>("general.memory.hugepage.defrag.game", "");

  auto disk_scheduler = cfg->get_key<std::string>("general.disk.scheduler.game", "");
  auto disk_read_ahead = cfg->get_key("general.disk.read-ahead.game", -1);
  auto disk_nr_requests = cfg->get_key("general.disk.nr-requests.game", -1);
  auto disk_rq_affinity = cfg->get_key("general.disk.rq-affinity.game", -1);

  change_cpu_governor(governor);
  change_cfs_parameter("sched_child_runs_first", sched_child_runs_first);
  set_hugepages(hugepage_state, hugepage_defrag);
  change_disk_parameter("scheduler", disk_scheduler);
  change_disk_parameter("read_ahead_kb", disk_read_ahead);
  change_disk_parameter("nr_requests", disk_nr_requests);
  change_disk_parameter("rq_affinity", disk_rq_affinity);

#ifdef USE_NVIDIA
  auto gpu_offset = cfg->get_key("general.nvidia.clock-offset.gpu.game", 0);
  auto memory_offset = cfg->get_key("general.nvidia.clock-offset.memory.game", 0);
  auto powermizer_mode = cfg->get_key<std::string>("general.nvidia.powermizer-mode.game", "auto");
  auto power_limit = cfg->get_key("general.nvidia.power-limit.game", -1);

  nvidia->set_powermizer_mode(0, powermizer_mode);
  nvidia->set_clock_offset(0, gpu_offset, memory_offset);
  nvidia->nvml->set_power_limit(0, power_limit);
#endif
}

void Tweaks::apply_process(const std::string& game, const int& pid, const std::string& thread_name) {
  change_niceness(game, pid, thread_name);
  change_scheduler_policy(game, pid, thread_name);
  change_iopriority(game, pid, thread_name);

  scheduler->set_affinity(pid,
                          cfg->get_key_array<int>("games." + game + ".threads.names." + thread_name + ".cpu-affinity"));
}

void Tweaks::remove() {
  auto governor = cfg->get_key<std::string>("general.cpu.governor.default", "schedutil");
  auto sched_child_runs_first = cfg->get_key("general.cpu.scheduler.sched_child_runs_first.default", -1);

  auto hugepage_state = cfg->get_key<std::string>("general.memory.hugepage.default", "");
  auto hugepage_defrag = cfg->get_key<std::string>("general.memory.hugepage.defrag.default", "");

  auto disk_scheduler = cfg->get_key<std::string>("general.disk.scheduler.default", "");
  auto disk_read_ahead = cfg->get_key("general.disk.read-ahead.default", -1);
  auto disk_nr_requests = cfg->get_key("general.disk.nr-requests.default", -1);
  auto disk_rq_affinity = cfg->get_key("general.disk.rq-affinity.default", -1);

  change_cpu_governor(governor);
  change_cfs_parameter("sched_child_runs_first", sched_child_runs_first);
  set_hugepages(hugepage_state, hugepage_defrag);
  change_disk_parameter("scheduler", disk_scheduler);
  change_disk_parameter("read_ahead_kb", disk_read_ahead);
  change_disk_parameter("nr_requests", disk_nr_requests);
  change_disk_parameter("rq_affinity", disk_rq_affinity);

#ifdef USE_NVIDIA
  auto gpu_offset = cfg->get_key("general.nvidia.clock-offset.gpu.default", 0);
  auto memory_offset = cfg->get_key("general.nvidia.clock-offset.memory.default", 0);
  auto powermizer_mode = cfg->get_key<std::string>("general.nvidia.powermizer-mode.default", "auto");
  auto power_limit = cfg->get_key("general.nvidia.power-limit.default", -1);

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

void Tweaks::change_cfs_parameter(const std::string& name, const int& value) {
  if (value == -1) {
    return;
  }

  auto path = fs::path("/proc/sys/kernel/" + name);

  std::ofstream f;

  f.open(path);

  f << value;

  f.close();

  std::cout << log_tag + "changed cfs parameter " + name + " to: " << value << std::endl;
}

void Tweaks::change_niceness(const std::string& game, const int& pid, const std::string& thread_name) {
  auto niceness = cfg->get_key("games." + game + ".threads.names." + thread_name + ".niceness", 0);

  setpriority(PRIO_PROCESS, pid, niceness);
}

void Tweaks::change_scheduler_policy(const std::string& game, const int& pid, const std::string& thread_name) {
  auto sched_policy =
      cfg->get_key<std::string>("games." + game + ".threads.names." + thread_name + ".scheduler-policy", "SCHED_OTHER");

  auto sched_priority =
      cfg->get_key("games." + game + ".threads.names." + thread_name + ".scheduler-policy-priority", 0);

  scheduler->set_policy(pid, sched_policy, sched_priority);
}

void Tweaks::change_iopriority(const std::string& game, const int& pid, const std::string& thread_name) {
  auto io_class = cfg->get_key<std::string>("games." + game + ".threads.names." + thread_name + ".io-class", "BE");
  auto io_priority = cfg->get_key("games." + game + ".threads.names." + thread_name + ".io-priority", 7);

  ioprio_set(pid, io_class, io_priority);
}

void Tweaks::set_hugepages(const std::string& state, const std::string& defrag) {
  if (state == "") {
    return;
  }

  auto path = fs::path("/sys/kernel/mm/transparent_hugepage/enabled");

  std::ofstream f;

  f.open(path);

  f << state;

  std::cout << log_tag + "changed transparent hugepage state to: " << state << std::endl;

  f.close();

  if (defrag == "") {
    return;
  }

  path = fs::path("/sys/kernel/mm/transparent_hugepage/defrag");

  f.open(path);

  f << defrag;

  std::cout << log_tag + "changed transparent hugepage defrag method to: " << defrag << std::endl;

  f.close();
}