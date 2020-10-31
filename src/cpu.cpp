#include "cpu.hpp"
#include <glibmm/i18n.h>
#include <gtkmm/checkbutton.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <thread>
#include "util.hpp"

namespace fs = std::filesystem;

Cpu::Cpu(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder, Application* application)
    : Gtk::Grid(cobject), app(application) {
  // loading glade widgets

  builder->get_widget("use_sched_batch", use_sched_batch);
  builder->get_widget("affinity_flowbox", affinity_flowbox);

  // power_cap = Glib::RefPtr<Gtk::Adjustment>::cast_dynamic(builder->get_object("power_cap"));

  // initializing widgets

  const auto n_cores = std::thread::hardware_concurrency();

  util::debug(log_tag + "number of cpu cores: " + std::to_string(n_cores));

  for (uint n = 0; n < n_cores; n++) {
    auto* checkbutton = Gtk::make_managed<Gtk::CheckButton>(std::to_string(n));

    checkbutton->set_active(true);

    affinity_flowbox->add(*checkbutton);
  }

  // signals connection

  // button_add->signal_clicked().connect([=]() { liststore->append(); });
}

Cpu::~Cpu() {
  util::debug(log_tag + "destroyed");
}

void Cpu::add_to_stack(Gtk::Stack* stack, Application* app) {
  auto builder = Gtk::Builder::create_from_resource("/com/github/wwmm/fastgame/ui/cpu.glade");

  Cpu* ui = nullptr;

  builder->get_widget_derived("widgets_grid", ui, app);

  stack->add(*ui, "cpu", _("CPU"));
}

void Cpu::find_hwmon_index() {
  auto path = fs::path("/sys/class/drm/card/device/hwmon/");

  for (const auto& entry : fs::directory_iterator(path)) {
    auto child_directory = fs::path(entry).filename().string();

    // It is unlikely that a system has more than 10 hwmon devices so we just get the last character

    std::string index_str(1, child_directory.back());

    util::debug(log_tag + "hwmon device index: " + index_str);
  }
}

void Cpu::read_power_cap() {
  auto path = fs::path("/sys/class/drm/card/power1_cap");

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

    // power_cap->set_value(power_cap_in_watts);

    util::debug(log_tag + "current power cap: " + std::to_string(power_cap_in_watts) + " W");
  }
}

void Cpu::read_power_cap_max() {
  auto path = fs::path("/sys/class/drm/card/power1_cap_max");

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

    // power_cap->set_upper(power_cap_in_watts);

    util::debug(log_tag + "maximum allowed power cap: " + std::to_string(power_cap_in_watts) + " W");
  }
}

void Cpu::read_performance_level() {
  auto path = fs::path("/sys/class/drm/card/device/power_dpm_force_performance_level");

  if (!fs::is_regular_file(path)) {
    util::debug(log_tag + "file " + path.string() + " does not exist!");
    util::debug(log_tag + "could not change the performance level!");
  } else {
    std::ifstream f;

    f.open(path, std::ios::in);

    std::string level;

    f >> level;

    f.close();

    util::debug(log_tag + "current performance level: " + level);
  }
}