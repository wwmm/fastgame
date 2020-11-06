#ifndef CPU_HPP
#define CPU_HPP

#include <gtkmm/adjustment.h>
#include <gtkmm/builder.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/flowbox.h>
#include <gtkmm/grid.h>
#include <gtkmm/stack.h>
#include <gtkmm/switch.h>

class Cpu : public Gtk::Grid {
 public:
  Cpu(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder);
  Cpu(const Cpu&) = delete;
  auto operator=(const Cpu&) -> Cpu& = delete;
  Cpu(const Cpu&&) = delete;
  auto operator=(const Cpu &&) -> Cpu& = delete;
  ~Cpu() override;

  static auto add_to_stack(Gtk::Stack* stack) -> Cpu*;

  auto get_enable_batch_scheduler() -> bool;

  void set_enable_batch_scheduler(const bool& state);

  auto get_child_runs_first() -> bool;

  void set_child_runs_first(const bool& state);

  auto get_frequency_governor() -> std::string;

  void set_frequency_governor(const std::string& name);

  auto get_cores() -> std::vector<std::string>;

  void set_cores(const std::vector<std::string>& list);

  auto get_use_cpu_dma_latency() -> bool;

  void set_use_cpu_dma_latency(const bool& state);

  auto get_use_realtime_wineserver() -> bool;

  void set_use_realtime_wineserver(const bool& state);

  auto get_niceness() -> int;

  void set_niceness(const int& value);

 private:
  std::string log_tag = "cpu: ";

  uint n_cores = 1;

  Gtk::Switch *use_sched_batch = nullptr, *child_runs_first = nullptr, *use_cpu_dma_latency = nullptr,
              *realtime_wineserver = nullptr;

  Gtk::ComboBoxText* frequency_governor = nullptr;

  Gtk::FlowBox* affinity_flowbox = nullptr;

  Glib::RefPtr<Gtk::Adjustment> niceness;
};

#endif
