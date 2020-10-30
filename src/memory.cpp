#include "memory.hpp"
#include <glibmm/i18n.h>
#include <filesystem>
#include <fstream>
#include <string>
#include "util.hpp"

namespace fs = std::filesystem;

Memory::Memory(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder, Application* application)
    : Gtk::Grid(cobject), app(application) {
  // loading glade widgets

  builder->get_widget("enabled", enabled);
  builder->get_widget("defrag", defrag);

  power_cap = Glib::RefPtr<Gtk::Adjustment>::cast_dynamic(builder->get_object("power_cap"));

  // initializing widgets

  read_transparent_huge_page_values();

  // signals connection

  // button_add->signal_clicked().connect([=]() { liststore->append(); });
}

Memory::~Memory() {
  util::debug(log_tag + "destroyed");
}

void Memory::add_to_stack(Gtk::Stack* stack, Application* app) {
  auto builder = Gtk::Builder::create_from_resource("/com/github/wwmm/fastgame/ui/memory.glade");

  Memory* ui = nullptr;

  builder->get_widget_derived("widgets_grid", ui, app);

  stack->add(*ui, "memory", _("Memory"));
}

void Memory::read_transparent_huge_page_values() {
  // parameter: enabled

  auto [enabled_list, enabled_value] = util::read_system_file_options("/sys/kernel/mm/transparent_hugepage/enabled");

  util::debug(log_tag + "transparent huge pages state: " + enabled_value);

  for (auto& value : enabled_list) {
    enabled->append(value);
  }

  enabled->set_active_text(enabled_value);

  // parameter: defrag

  auto [defrag_list, defrag_value] = util::read_system_file_options("/sys/kernel/mm/transparent_hugepage/defrag");

  util::debug(log_tag + "transparent huge pages defrag: " + defrag_value);

  for (auto& value : defrag_list) {
    defrag->append(value);
  }

  defrag->set_active_text(defrag_value);
}

void Memory::read_power_cap() {
  auto path = fs::path("/sys/class/drm/card/device/hwmon/hwmon/power1_cap");

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

void Memory::read_performance_level() {
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

    // if (level == "auto") {
    //   performance_level->set_active(0);
    // } else if (level == "low") {
    //   performance_level->set_active(1);
    // } else if (level == "high") {
    //   performance_level->set_active(2);
    // } else if (level == "manual") {
    //   performance_level->set_active(3);
    // } else if (level == "profile_standard") {
    //   performance_level->set_active(4);
    // } else if (level == "profile_min_sclk") {
    //   performance_level->set_active(5);
    // } else if (level == "profile_min_mclk") {
    //   performance_level->set_active(6);
    // } else if (level == "profile_peak") {
    //   performance_level->set_active(7);
    // }
  }
}