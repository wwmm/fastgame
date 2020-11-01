#ifndef CPU_HPP
#define CPU_HPP

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

 private:
  std::string log_tag = "cpu: ";

  uint n_cores = 1;

  Gtk::Switch *use_sched_batch = nullptr, *child_runs_first = nullptr;

  Gtk::ComboBoxText* frequency_governor = nullptr;

  Gtk::FlowBox* affinity_flowbox = nullptr;
};

#endif
