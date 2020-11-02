#ifndef AMDGPU_HPP
#define AMDGPU_HPP

#include <gtkmm/adjustment.h>
#include <gtkmm/builder.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/grid.h>
#include <gtkmm/stack.h>

class Amdgpu : public Gtk::Grid {
 public:
  Amdgpu(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder);
  Amdgpu(const Amdgpu&) = delete;
  auto operator=(const Amdgpu&) -> Amdgpu& = delete;
  Amdgpu(const Amdgpu&&) = delete;
  auto operator=(const Amdgpu &&) -> Amdgpu& = delete;
  ~Amdgpu() override;

  static auto add_to_stack(Gtk::Stack* stack) -> Amdgpu*;

  auto get_performance_level() -> std::string;

  void set_performance_level(const std::string& level);

  auto get_power_cap() -> int;

  void set_power_cap(const int& value);

 private:
  std::string log_tag = "amdgpu: ";

  bool found_gpu = false;
  uint card_index = 0, hwmon_index = 0;

  Gtk::ComboBoxText* performance_level = nullptr;

  Glib::RefPtr<Gtk::Adjustment> power_cap;

  void find_hwmon_index();
  void read_power_cap();
  void read_power_cap_max();
  void read_performance_level();
};

#endif
