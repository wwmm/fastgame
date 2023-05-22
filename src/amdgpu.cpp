#include "amdgpu.hpp"

namespace ui::amdgpu {

using namespace std::string_literals;

auto constexpr log_tag = "amdgpu: ";

uint n_cards = 0;

struct _Amdgpu {
  GtkPopover parent_instance;

  GtkDropDown *performance_level0, *power_profile0, *performance_level1, *power_profile1;

  GtkSpinButton *power_cap0, *power_cap1;
};

G_DEFINE_TYPE(Amdgpu, amdgpu, GTK_TYPE_BOX)

auto get_n_cards() -> int {
  return n_cards;
}

void count_cards() {
  n_cards = 0;

  uint n_subfolders = 0;

  auto dir_path = std::filesystem::path("/sys/class/drm/");

  if (std::filesystem::is_directory(dir_path)) {
    for (const auto& entry : std::filesystem::directory_iterator(dir_path)) {
      if (std::filesystem::is_directory(entry)) {
        if (util::card_is_amdgpu(n_subfolders)) {
          n_cards++;
        }

        n_subfolders++;
      }
    }
  }
}

void set_performance_level(Amdgpu* self, const std::string& name, const int& card_index) {
  auto* dropdown = (card_index == 0) ? self->performance_level0 : self->performance_level1;

  auto* model = reinterpret_cast<GtkStringList*>(gtk_drop_down_get_model(dropdown));

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

  gtk_drop_down_set_selected(dropdown, id);
}

auto get_performance_level(Amdgpu* self, const int& card_index) -> std::string {
  auto* dropdown = (card_index == 0) ? self->performance_level0 : self->performance_level1;

  auto* selected_item = gtk_drop_down_get_selected_item(dropdown);

  if (selected_item == nullptr) {
    return "";
  }

  return gtk_string_object_get_string(GTK_STRING_OBJECT(selected_item));
}

void set_power_profile(Amdgpu* self, const std::string& id, const int& card_index) {
  auto* dropdown = (card_index == 0) ? self->power_profile0 : self->power_profile1;

  uint idx = 0;

  util::str_to_num(id, idx);

  gtk_drop_down_set_selected(dropdown, idx);
}

auto get_power_profile(Amdgpu* self, const int& card_index) -> std::string {
  auto* dropdown = (card_index == 0) ? self->power_profile0 : self->power_profile1;

  return util::to_string(gtk_drop_down_get_selected(dropdown));
}

void set_power_cap(Amdgpu* self, const int& value, const int& card_index) {
  auto* button = (card_index == 0) ? self->power_cap0 : self->power_cap1;

  gtk_spin_button_set_value(button, value);
}

auto get_power_cap(Amdgpu* self, const int& card_index) -> int {
  auto* button = (card_index == 0) ? self->power_cap0 : self->power_cap1;

  return static_cast<int>(gtk_spin_button_get_value(button));
}

void read_power_cap_max(Amdgpu* self, const int& card_index) {
  auto hwmon_index = util::find_hwmon_index(card_index);

  auto path = std::filesystem::path("/sys/class/drm/card" + std::to_string(card_index) + "/device/hwmon/hwmon" +
                                    std::to_string(hwmon_index) + "/power1_cap_max");

  if (!std::filesystem::is_regular_file(path)) {
    util::debug(log_tag + "file "s + path.string() + " does not exist!");
    util::debug(log_tag + "card "s + util::to_string(card_index) + " could not read the maximum power cap!"s);
  } else {
    std::ifstream f;

    f.open(path, std::ios::in);

    int raw_value = 0;  // microWatts

    f >> raw_value;

    f.close();

    int power_cap_in_watts = raw_value / 1000000;

    GtkAdjustment* adj = nullptr;

    switch (card_index) {
      case 0:
        adj = gtk_spin_button_get_adjustment(self->power_cap0);

      case 1:
        adj = gtk_spin_button_get_adjustment(self->power_cap1);
    }

    gtk_adjustment_set_upper(adj, power_cap_in_watts);

    util::debug(log_tag + "card "s + util::to_string(card_index) + " maximum allowed power cap: "s +
                std::to_string(power_cap_in_watts) + " W");
  }
}

void read_power_cap(Amdgpu* self, const int& card_index) {
  auto* spinbutton = (card_index == 0) ? self->power_cap0 : self->power_cap1;

  auto hwmon_index = util::find_hwmon_index(card_index);

  auto path = std::filesystem::path("/sys/class/drm/card" + std::to_string(card_index) + "/device/hwmon/hwmon" +
                                    std::to_string(hwmon_index) + "/power1_cap");

  if (!std::filesystem::is_regular_file(path)) {
    gtk_widget_set_sensitive(GTK_WIDGET(spinbutton), 0);

    util::debug(log_tag + "file "s + path.string() + " does not exist!");
    util::debug(log_tag + "card "s + util::to_string(card_index) + " could not change the power cap!"s);
  } else {
    std::ifstream f;

    f.open(path, std::ios::in);

    int raw_value = 0;  // microWatts

    f >> raw_value;

    f.close();

    int power_cap_in_watts = raw_value / 1000000;

    gtk_spin_button_set_value(spinbutton, power_cap_in_watts);

    util::debug(log_tag + "card "s + util::to_string(card_index) + " current power cap: "s +
                std::to_string(power_cap_in_watts) + " W");
  }
}

void read_performance_level(Amdgpu* self, const int& card_index) {
  auto path = std::filesystem::path("/sys/class/drm/card" + std::to_string(card_index) +
                                    "/device/power_dpm_force_performance_level");

  if (!std::filesystem::is_regular_file(path)) {
    util::debug(log_tag + "file "s + path.string() + " does not exist!");
    util::debug(log_tag + "card "s + util::to_string(card_index) + " could not read the power profile!"s);
  } else {
    std::ifstream f;

    f.open(path, std::ios::in);

    std::string level;

    f >> level;

    f.close();

    set_performance_level(self, level);

    util::debug(log_tag + "card "s + util::to_string(card_index) + " current performance level: "s + level);
  }
}

void read_power_profile(Amdgpu* self, const int& card_index) {
  auto path =
      std::filesystem::path("/sys/class/drm/card" + std::to_string(card_index) + "/device/pp_power_profile_mode");

  auto dropdown = (card_index == 0) ? self->power_profile0 : self->power_profile1;

  if (!std::filesystem::is_regular_file(path)) {
    gtk_widget_set_sensitive(GTK_WIDGET(dropdown), 0);

    util::debug(log_tag + "file "s + path.string() + " does not exist!");
    util::debug(log_tag + "card "s + util::to_string(card_index) + " could not change the performance level!"s);
  } else {
    std::ifstream f;

    f.open(path, std::ios::in);

    std::stringstream buffer;

    buffer << f.rdbuf();

    f.close();

    auto* model = reinterpret_cast<GtkStringList*>(gtk_drop_down_get_model(dropdown));

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

            util::debug(log_tag + "card "s + util::to_string(card_index) + " current power profile: "s + word);

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

          gtk_drop_down_set_selected(dropdown, id);
        }
      }
    }
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

  gtk_widget_class_bind_template_child(widget_class, Amdgpu, performance_level0);
  gtk_widget_class_bind_template_child(widget_class, Amdgpu, power_profile0);
  gtk_widget_class_bind_template_child(widget_class, Amdgpu, power_cap0);

  gtk_widget_class_bind_template_child(widget_class, Amdgpu, performance_level1);
  gtk_widget_class_bind_template_child(widget_class, Amdgpu, power_profile1);
  gtk_widget_class_bind_template_child(widget_class, Amdgpu, power_cap1);
}

void amdgpu_init(Amdgpu* self) {
  gtk_widget_init_template(GTK_WIDGET(self));

  ui::prepare_spinbuttons<"W">(self->power_cap0, self->power_cap1);

  count_cards();

  util::debug(log_tag + "number of amdgpu cards: "s + util::to_string(n_cards));

  for (uint n = 0; n < n_cards; n++) {
    auto* performance_level = (n == 0) ? self->performance_level0 : self->performance_level1;

    struct Data {
      uint idx;
      Amdgpu* self;
    };

    auto* data = new Data{.idx = n, .self = self};

    g_signal_connect_data(
        performance_level, "notify::selected-item",
        G_CALLBACK(+[](GtkDropDown* dropdown, GParamSpec* pspec, Data* data) {
          if (auto selected_item = gtk_drop_down_get_selected_item(dropdown); selected_item != nullptr) {
            auto* level = gtk_string_object_get_string(GTK_STRING_OBJECT(selected_item));

            if (std::strcmp(level, "manual") == 0) {
              gtk_widget_set_sensitive(GTK_WIDGET(data->self->power_profile0), 1);
            } else {
              gtk_widget_set_sensitive(GTK_WIDGET(data->self->power_profile1), 0);
            }
          }
        }),
        data, (GClosureNotify) + [](Data* data, GClosure* closure) { delete data; }, G_CONNECT_DEFAULT);

    read_power_cap_max(self, n);
    read_power_cap(self, n);
    read_performance_level(self, n);
    read_power_profile(self, n);

    auto* power_profile = (n == 0) ? self->power_profile0 : self->power_profile1;

    if (get_performance_level(self, n) == "manual") {
      gtk_widget_set_sensitive(GTK_WIDGET(power_profile), 1);
    } else {
      gtk_widget_set_sensitive(GTK_WIDGET(power_profile), 0);
    }
  }
}

auto create() -> Amdgpu* {
  return static_cast<Amdgpu*>(g_object_new(FG_TYPE_AMDGPU, nullptr));
}

}  // namespace ui::amdgpu
