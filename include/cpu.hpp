#ifndef CPU_HPP
#define CPU_HPP

#include <gtkmm/adjustment.h>
#include <gtkmm/builder.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/flowbox.h>
#include <gtkmm/grid.h>
#include <gtkmm/stack.h>
#include <gtkmm/switch.h>
#include "application.hpp"

class Cpu : public Gtk::Grid {
 public:
  Cpu(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder, Application* application);
  Cpu(const Cpu&) = delete;
  auto operator=(const Cpu&) -> Cpu& = delete;
  Cpu(const Cpu&&) = delete;
  auto operator=(const Cpu &&) -> Cpu& = delete;
  ~Cpu() override;

  static void add_to_stack(Gtk::Stack* stack, Application* app);

 private:
  std::string log_tag = "cpu: ";

  uint n_cores = 1;

  Application* app = nullptr;

  Gtk::Switch* use_sched_batch = nullptr;

  Gtk::ComboBoxText* frequency_governor = nullptr;

  Gtk::FlowBox* affinity_flowbox = nullptr;

  // Glib::RefPtr<Gtk::Adjustment> power_cap;

  void find_hwmon_index();
  void read_power_cap();
  void read_power_cap_max();
  void read_performance_level();
};

#endif
