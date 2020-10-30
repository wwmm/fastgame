#include "amdgpu.hpp"
#include <glibmm/i18n.h>
#include <filesystem>
#include <fstream>
#include <string>
#include "util.hpp"

namespace fs = std::filesystem;

Amdgpu::Amdgpu(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder, Application* application)
    : Gtk::Grid(cobject), app(application) {
  // loading glade widgets

  builder->get_widget("performance_level", performance_level);

  power_cap = Glib::RefPtr<Gtk::Adjustment>::cast_dynamic(builder->get_object("power_cap"));

  // initializing widgets

  if (fs::is_directory("/sys/class/drm/card" + std::to_string(card_index))) {
    found_gpu = true;

    util::debug(log_tag + "using the card at the index: 0");

    find_hwmon_index();
    read_power_cap_max();
    read_power_cap();
    read_performance_level();
  }

  // signals connection

  // button_add->signal_clicked().connect([=]() { liststore->append(); });
}

Amdgpu::~Amdgpu() {
  util::debug(log_tag + "destroyed");
}

void Amdgpu::add_to_stack(Gtk::Stack* stack, Application* app) {
  auto builder = Gtk::Builder::create_from_resource("/com/github/wwmm/fastgame/ui/amdgpu.glade");

  Amdgpu* ui = nullptr;

  builder->get_widget_derived("widgets_grid", ui, app);

  stack->add(*ui, "amdgpu", _("AMDGPU"));
}

void Amdgpu::find_hwmon_index() {
  auto path = fs::path("/sys/class/drm/card" + std::to_string(card_index) + "/device/hwmon/");

  for (const auto& entry : fs::directory_iterator(path)) {
    auto child_directory = fs::path(entry).filename().string();

    // It is unlikely that a system has more than 10 hwmon devices so we just get the last character

    std::string index_str(1, child_directory.back());

    hwmon_index = std::stoi(index_str);

    util::debug(log_tag + "hwmon device index: " + index_str);
  }
}

void Amdgpu::read_power_cap() {
  if (found_gpu) {
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
}

void Amdgpu::read_power_cap_max() {
  if (found_gpu) {
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
}

void Amdgpu::read_performance_level() {
  if (found_gpu) {
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

      util::debug(log_tag + "current performance level: " + level);

      if (level == "auto") {
        performance_level->set_active(0);
      } else if (level == "low") {
        performance_level->set_active(1);
      } else if (level == "high") {
        performance_level->set_active(2);
      } else if (level == "manual") {
        performance_level->set_active(3);
      } else if (level == "profile_standard") {
        performance_level->set_active(4);
      } else if (level == "profile_min_sclk") {
        performance_level->set_active(5);
      } else if (level == "profile_min_mclk") {
        performance_level->set_active(6);
      } else if (level == "profile_peak") {
        performance_level->set_active(7);
      }
    }
  }
}