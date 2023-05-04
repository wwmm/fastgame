#include "amdgpu.hpp"

namespace ui::amdgpu {

using namespace std::string_literals;

auto constexpr log_tag = "amdgpu: ";

uint card_index = 0, hwmon_index = 0;

struct _Amdgpu {
  GtkPopover parent_instance;

  GtkDropDown *performance_level, *power_profile;

  GtkSpinButton* power_cap;
};

G_DEFINE_TYPE(Amdgpu, amdgpu, GTK_TYPE_BOX)

auto get_card_index(Amdgpu* self) -> int {
  return card_index;
}

void set_performance_level(Amdgpu* self, const std::string& name) {
  auto* model = reinterpret_cast<GtkStringList*>(gtk_drop_down_get_model(self->performance_level));

  guint id = 0;

  for (guint n = 0U; n < g_list_model_get_n_items(G_LIST_MODEL(model)); n++) {
    auto label = gtk_string_list_get_string(model, n);

    if (label == nullptr) {
      continue;
    }

    if (label == name) {
      id = n;
    }
  }

  gtk_drop_down_set_selected(self->performance_level, id);
}

auto get_performance_level(Amdgpu* self) -> std::string {
  auto* selected_item = gtk_drop_down_get_selected_item(self->performance_level);

  if (selected_item == nullptr) {
    return "";
  }

  return gtk_string_object_get_string(GTK_STRING_OBJECT(selected_item));
}

void set_power_profile(Amdgpu* self, const std::string& id) {
  uint idx = 0;

  util::str_to_num(id, idx);

  gtk_drop_down_set_selected(self->power_profile, idx);
}

auto get_power_profile(Amdgpu* self) -> std::string {
  return util::to_string(gtk_drop_down_get_selected(self->power_profile));
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

    set_performance_level(self, level);

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

    auto* model = reinterpret_cast<GtkStringList*>(gtk_drop_down_get_model(self->power_profile));

    gtk_string_list_splice(model, 0, g_list_model_get_n_items(G_LIST_MODEL(model)), nullptr);

    std::string line;

    std::getline(buffer, line);  // Discarding the header

    while (std::getline(buffer, line)) {
      if (line.ends_with(":")) {
        line.pop_back();

        std::istringstream ss(line);

        uint count = 0;

        std::string word;
        std::string profile;
        std::string id_str;
        bool is_selected = false;

        while (ss >> word) {
          if (word.ends_with("*")) {
            word.pop_back();

            util::debug(log_tag + "current power profile: "s + word);

            is_selected = true;
          }

          if (count == 0) {
            id_str = word;
          }

          if (count == 1) {
            profile = word;
          }

          count++;
        }

        // util::info(id + " -> " + profile);

        gtk_string_list_append(model, profile.c_str());

        if (is_selected) {
          uint id = 0;

          util::str_to_num(id_str, id);

          gtk_drop_down_set_selected(self->power_profile, id);
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
