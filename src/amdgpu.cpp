#include "amdgpu.hpp"

namespace ui::amdgpu {

using namespace std::string_literals;

auto constexpr log_tag = "amdgpu: ";

uint card_index = 0, hwmon_index = 0;

struct _Amdgpu {
  GtkPopover parent_instance;

  GtkComboBoxText *performance_level, *power_profile;

  GtkSpinButton* power_cap;
};

G_DEFINE_TYPE(Amdgpu, amdgpu, GTK_TYPE_BOX)

auto get_card_index(Amdgpu* self) -> int {
  return card_index;
}

void set_performance_level(Amdgpu* self, const std::string& name) {
  gtk_combo_box_set_active_id(GTK_COMBO_BOX(self->performance_level), name.c_str());
}

auto get_performance_level(Amdgpu* self) -> std::string {
  return gtk_combo_box_text_get_active_text(self->performance_level);
}

void set_power_profile(Amdgpu* self, const std::string& id) {
  gtk_combo_box_set_active_id(GTK_COMBO_BOX(self->power_profile), id.c_str());
}

auto get_power_profile(Amdgpu* self) -> std::string {
  return gtk_combo_box_get_active_id(GTK_COMBO_BOX(self->power_profile));
}

void set_power_cap(Amdgpu* self, const int& value) {
  gtk_spin_button_set_value(self->power_cap, value);
}

auto get_power_cap(Amdgpu* self) -> int {
  return static_cast<int>(gtk_spin_button_get_value(self->power_cap));
}

void read_power_cap_max(Amdgpu* self) {
  auto path = std::filesystem::path("/sys/class/drm/card" + std::to_string(card_index) + "/device/hwmon/hwmon" +
                                    std::to_string(hwmon_index) + "/power1_cap_max");

  if (!std::filesystem::is_regular_file(path)) {
    util::debug(log_tag + "file "s + path.string() + " does not exist!");
    util::debug(log_tag + "could not read the maximum power cap!"s);
  } else {
    std::ifstream f;

    f.open(path, std::ios::in);

    int raw_value = 0;  // microWatts

    f >> raw_value;

    f.close();

    int power_cap_in_watts = raw_value / 1000000;

    auto adj = gtk_spin_button_get_adjustment(self->power_cap);

    gtk_adjustment_set_upper(adj, power_cap_in_watts);

    util::debug(log_tag + "maximum allowed power cap: "s + std::to_string(power_cap_in_watts) + " W");
  }
}

void read_power_cap(Amdgpu* self) {
  auto path = std::filesystem::path("/sys/class/drm/card" + std::to_string(card_index) + "/device/hwmon/hwmon" +
                                    std::to_string(hwmon_index) + "/power1_cap");

  if (!std::filesystem::is_regular_file(path)) {
    util::debug(log_tag + "file "s + path.string() + " does not exist!");
    util::debug(log_tag + "could not change the power cap!"s);
  } else {
    std::ifstream f;

    f.open(path, std::ios::in);

    int raw_value = 0;  // microWatts

    f >> raw_value;

    f.close();

    int power_cap_in_watts = raw_value / 1000000;

    gtk_spin_button_set_value(self->power_cap, power_cap_in_watts);

    util::debug(log_tag + "current power cap: "s + std::to_string(power_cap_in_watts) + " W");
  }
}

void read_performance_level(Amdgpu* self) {
  auto path = std::filesystem::path("/sys/class/drm/card" + std::to_string(card_index) +
                                    "/device/power_dpm_force_performance_level");

  if (!std::filesystem::is_regular_file(path)) {
    util::debug(log_tag + "file "s + path.string() + " does not exist!");
    util::debug(log_tag + "could not read the power profile!"s);
  } else {
    std::ifstream f;

    f.open(path, std::ios::in);

    std::string level;

    f >> level;

    f.close();

    gtk_combo_box_set_active_id(GTK_COMBO_BOX(self->performance_level), level.c_str());

    util::debug(log_tag + "current performance level: "s + level);
  }
}

void read_power_profile(Amdgpu* self) {
  auto path =
      std::filesystem::path("/sys/class/drm/card" + std::to_string(card_index) + "/device/pp_power_profile_mode");

  if (!std::filesystem::is_regular_file(path)) {
    util::debug(log_tag + "file "s + path.string() + " does not exist!");
    util::debug(log_tag + "could not change the performance level!"s);
  } else {
    std::ifstream f;

    f.open(path, std::ios::in);

    std::stringstream buffer;

    buffer << f.rdbuf();

    f.close();

    std::string line;

    std::getline(buffer, line);  // Discarding the header

    while (std::getline(buffer, line)) {
      if (line.ends_with(":")) {
        line.pop_back();

        std::istringstream ss(line);

        uint count = 0;

        std::string word;
        std::string profile;
        std::string id;
        bool is_selected = false;

        while (ss >> word) {
          if (word.ends_with("*")) {
            word.pop_back();

            util::debug(log_tag + "current power profile: "s + word);

            is_selected = true;
          }

          if (count == 0) {
            id = word;
          }

          if (count == 1) {
            profile = word;
          }

          count++;
        }

        // util::info(id + " -> " + profile);

        gtk_combo_box_text_append(self->power_profile, id.c_str(), profile.c_str());

        if (is_selected) {
          gtk_combo_box_set_active_id(GTK_COMBO_BOX(self->power_profile), id.c_str());
        }
      }
    }
  }
}

void on_performance_level_changed(Amdgpu* self, GtkComboBox* combo) {
  if (get_performance_level(self) != "manual") {
    gtk_widget_set_sensitive(GTK_WIDGET(self->power_profile), 0);
  } else {
    gtk_widget_set_sensitive(GTK_WIDGET(self->power_profile), 1);
  }
}

void dispose(GObject* object) {
  util::debug(log_tag + "disposed"s);

  G_OBJECT_CLASS(amdgpu_parent_class)->dispose(object);
}

void finalize(GObject* object) {
  util::debug(log_tag + "finalized"s);

  G_OBJECT_CLASS(amdgpu_parent_class)->finalize(object);
}

void amdgpu_class_init(AmdgpuClass* klass) {
  auto* object_class = G_OBJECT_CLASS(klass);
  auto* widget_class = GTK_WIDGET_CLASS(klass);

  object_class->dispose = dispose;
  object_class->finalize = finalize;

  gtk_widget_class_set_template_from_resource(widget_class, "/com/github/wwmm/fastgame/ui/amdgpu.ui");

  gtk_widget_class_bind_template_child(widget_class, Amdgpu, performance_level);
  gtk_widget_class_bind_template_child(widget_class, Amdgpu, power_profile);
  gtk_widget_class_bind_template_child(widget_class, Amdgpu, power_cap);

  gtk_widget_class_bind_template_callback(widget_class, on_performance_level_changed);
}

void amdgpu_init(Amdgpu* self) {
  gtk_widget_init_template(GTK_WIDGET(self));

  ui::prepare_spinbutton<"W">(self->power_cap);

  util::debug(log_tag + "using the card at the index: 0"s);

  hwmon_index = util::find_hwmon_index(0);

  util::debug(log_tag + "hwmon device index: "s + std::to_string(hwmon_index));

  read_power_cap_max(self);
  read_power_cap(self);
  read_performance_level(self);
  read_power_profile(self);
}

auto create() -> Amdgpu* {
  return static_cast<Amdgpu*>(g_object_new(FG_TYPE_AMDGPU, nullptr));
}

}  // namespace ui::amdgpu
