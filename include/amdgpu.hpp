#ifndef AMDGPU_HPP
#define AMDGPU_HPP

#include <gtkmm/adjustment.h>
#include <gtkmm/builder.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/flowbox.h>
#include <gtkmm/grid.h>
#include <gtkmm/stack.h>

class Amdgpu : public Gtk::Grid {
 public:
  Amdgpu(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder);
  Amdgpu(const Amdgpu&) = delete;
  auto operator=(const Amdgpu&) -> Amdgpu& = delete;
  Amdgpu(const Amdgpu&&) = delete;
  auto operator=(const Amdgpu&&) -> Amdgpu& = delete;
  ~Amdgpu() override;

  static auto add_to_stack(Gtk::Stack* stack) -> Amdgpu*;

  auto get_performance_level() -> std::string;

  void set_performance_level(const std::string& level);

  auto get_power_cap() -> int;

  void set_power_cap(const int& value);

  auto get_card_index() const -> int;

  auto get_irq_affinity() const -> int;

  void set_irq_affinity(const int& core_index);

 private:
  std::string log_tag = "amdgpu: ";

  uint card_index = 0, hwmon_index = 0;

  Gtk::ComboBoxText* performance_level = nullptr;

  Gtk::FlowBox* irq_affinity_flowbox = nullptr;

  Glib::RefPtr<Gtk::Adjustment> power_cap;

  void read_power_cap();
  void read_power_cap_max();
  void read_performance_level();
  void read_irq_affinity();
};

#endif
