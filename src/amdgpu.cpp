#include "amdgpu.hpp"
#include <glibmm/i18n.h>
#include <gtkmm/radiobutton.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include "util.hpp"

namespace fs = std::filesystem;

Amdgpu::Amdgpu(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder) : Gtk::Grid(cobject) {
  // loading glade widgets

  builder->get_widget("performance_level", performance_level);
  builder->get_widget("irq_affinity_flowbox", irq_affinity_flowbox);

  power_cap = Glib::RefPtr<Gtk::Adjustment>::cast_dynamic(builder->get_object("power_cap"));

  // initializing widgets

  util::debug(log_tag + "using the card at the index: 0");

  hwmon_index = util::find_hwmon_index(0);

  util::debug(log_tag + "hwmon device index: " + std::to_string(hwmon_index));

  read_power_cap_max();
  read_power_cap();
  read_performance_level();
  read_irq_affinity();
}

Amdgpu::~Amdgpu() {
  util::debug(log_tag + "destroyed");
}

auto Amdgpu::add_to_stack(Gtk::Stack* stack) -> Amdgpu* {
  auto builder = Gtk::Builder::create_from_resource("/com/github/wwmm/fastgame/ui/amdgpu.glade");

  Amdgpu* ui = nullptr;

  builder->get_widget_derived("widgets_grid", ui);

  stack->add(*ui, "amdgpu", _("AMDGPU"));

  return ui;
}

auto Amdgpu::get_card_index() const -> int {
  return card_index;
}

void Amdgpu::read_power_cap() {
  auto path = fs::path("/sys/class/drm/card" + std::to_string(card_index) + "/device/hwmon/hwmon" +
                       std::to_string(hwmon_index) + "/power1_cap");

  if (!fs::is_regular_file(path)) {
    util::debug(log_tag + "file " + path.string() + " does not exist!");
    util::debug(log_tag + "could not change the power cap!");
  } else {
    std::ifstream f;

    f.open(path, std::ios::in);

    int raw_value = 0;  // microWatts

    f >> raw_value;

    f.close();

    int power_cap_in_watts = raw_value / 1000000;

    power_cap->set_value(power_cap_in_watts);

    util::debug(log_tag + "current power cap: " + std::to_string(power_cap_in_watts) + " W");
  }
}

void Amdgpu::read_power_cap_max() {
  auto path = fs::path("/sys/class/drm/card" + std::to_string(card_index) + "/device/hwmon/hwmon" +
                       std::to_string(hwmon_index) + "/power1_cap_max");

  if (!fs::is_regular_file(path)) {
    util::debug(log_tag + "file " + path.string() + " does not exist!");
    util::debug(log_tag + "could not read the maximum power cap!");
  } else {
    std::ifstream f;

    f.open(path, std::ios::in);

    int raw_value = 0;  // microWatts

    f >> raw_value;

    f.close();

    int power_cap_in_watts = raw_value / 1000000;

    power_cap->set_upper(power_cap_in_watts);

    util::debug(log_tag + "maximum allowed power cap: " + std::to_string(power_cap_in_watts) + " W");
  }
}

void Amdgpu::read_performance_level() {
  auto path =
      fs::path("/sys/class/drm/card" + std::to_string(card_index) + "/device/power_dpm_force_performance_level");

  if (!fs::is_regular_file(path)) {
    util::debug(log_tag + "file " + path.string() + " does not exist!");
    util::debug(log_tag + "could not change the performance level!");
  } else {
    std::ifstream f;

    f.open(path, std::ios::in);

    std::string level;

    f >> level;

    f.close();

    performance_level->set_active_text(level);

    util::debug(log_tag + "current performance level: " + level);
  }
}

void Amdgpu::read_irq_affinity() {
  int gpu_irq = util::get_irq_number("amdgpu");

  util::debug(log_tag + "gpu irq number: " + std::to_string(gpu_irq));

  uint current_core = util::get_irq_affinity(gpu_irq);

  uint n_cores = std::thread::hardware_concurrency();

  for (uint n = 0; n < n_cores; n++) {
    auto* radiobutton = Gtk::make_managed<Gtk::RadioButton>(std::to_string(n));

    if (n > 0) {
      auto children = irq_affinity_flowbox->get_children();
      auto* flowbox_child = dynamic_cast<Gtk::FlowBoxChild*>(children[0]);

      auto* first_radiobutton = dynamic_cast<Gtk::RadioButton*>(flowbox_child->get_child());

      radiobutton->join_group(*first_radiobutton);
    }

    if (n == current_core) {
      radiobutton->set_active(true);
    }

    irq_affinity_flowbox->add(*radiobutton);
  }
}

auto Amdgpu::get_performance_level() -> std::string {
  return performance_level->get_active_text();
}

void Amdgpu::set_performance_level(const std::string& level) {
  performance_level->set_active_text(level);
}

auto Amdgpu::get_power_cap() -> int {
  return static_cast<int>(power_cap->get_value());
}

void Amdgpu::set_power_cap(const int& value) {
  power_cap->set_value(value);
}

auto Amdgpu::get_irq_affinity() const -> int {
  auto children = irq_affinity_flowbox->get_children();

  for (auto& child : children) {
    auto* flowbox_child = dynamic_cast<Gtk::FlowBoxChild*>(child);

    auto* radiobutton = dynamic_cast<Gtk::RadioButton*>(flowbox_child->get_child());

    if (radiobutton->get_active()) {
      return std::stoi(radiobutton->get_label());
    }
  }

  return 0;
}

void Amdgpu::set_irq_affinity(const int& core_index) {
  auto children = irq_affinity_flowbox->get_children();

  for (auto& child : children) {
    auto* flowbox_child = dynamic_cast<Gtk::FlowBoxChild*>(child);

    auto* radiobutton = dynamic_cast<Gtk::RadioButton*>(flowbox_child->get_child());

    if (radiobutton->get_label() == std::to_string(core_index)) {
      radiobutton->set_active(true);

      break;
    }
  }
}