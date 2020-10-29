#ifndef AMDGPU_HPP
#define AMDGPU_HPP

#include <gtkmm/adjustment.h>
#include <gtkmm/builder.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/grid.h>
#include <gtkmm/stack.h>
#include "application.hpp"

class Amdgpu : public Gtk::Grid {
 public:
  Amdgpu(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder, Application* application);
  Amdgpu(const Amdgpu&) = delete;
  auto operator=(const Amdgpu&) -> Amdgpu& = delete;
  Amdgpu(const Amdgpu&&) = delete;
  auto operator=(const Amdgpu &&) -> Amdgpu& = delete;
  ~Amdgpu() override;

  static void add_to_stack(Gtk::Stack* stack, Application* app);

 private:
  std::string log_tag = "amdgpu: ";

  bool found_gpu = false;
  uint card_index = 0, hwmon_index = 0;

  Application* app = nullptr;

  Gtk::ComboBoxText* performance_level = nullptr;

  Glib::RefPtr<Gtk::Adjustment> power_cap;

  std::vector<sigc::connection> connections;

  void find_hwmon_index();
  void read_power_cap();
};

#endif
