#include "nvidia.hpp"

namespace ui::nvidia {

using namespace std::string_literals;

auto constexpr log_tag = "nvidia: ";

uint n_cards = 0;

#ifdef USE_NVIDIA
std::unique_ptr<nvidia_wrapper::Nvidia> nv_wrapper;
#endif

struct _Nvidia {
  GtkPopover parent_instance;

  GtkDropDown *performance_level0, *power_profile0;

  GtkSpinButton* power_cap0;
};

G_DEFINE_TYPE(Nvidia, nvidia, GTK_TYPE_BOX)

auto has_gpu() -> bool {
  bool found = false;

#ifdef USE_NVIDIA
  found = nv_wrapper->has_gpu();
#endif

  return found;
}

auto get_n_cards() -> int {
  return n_cards;
}

void set_performance_level(Nvidia* self, const std::string& name, const int& card_index) {
  auto* model = reinterpret_cast<GtkStringList*>(gtk_drop_down_get_model(self->performance_level0));

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

  gtk_drop_down_set_selected(self->performance_level0, id);
}

auto get_performance_level(Nvidia* self, const int& card_index) -> std::string {
  auto* selected_item = gtk_drop_down_get_selected_item(self->performance_level0);

  if (selected_item == nullptr) {
    return "";
  }

  return gtk_string_object_get_string(GTK_STRING_OBJECT(selected_item));
}

void set_power_profile(Nvidia* self, const std::string& id, const int& card_index) {
  uint idx = 0;

  util::str_to_num(id, idx);

  gtk_drop_down_set_selected(self->power_profile0, idx);
}

auto get_power_profile(Nvidia* self, const int& card_index) -> std::string {
  return util::to_string(gtk_drop_down_get_selected(self->power_profile0));
}

void set_power_cap(Nvidia* self, const int& value, const int& card_index) {
  gtk_spin_button_set_value(self->power_cap0, value);
}

auto get_power_cap(Nvidia* self, const int& card_index) -> int {
  return static_cast<int>(gtk_spin_button_get_value(self->power_cap0));
}

void read_power_cap_max(Nvidia* self) {
  // auto hwmon_index = util::find_hwmon_index(card_index);

  // auto path = std::filesystem::path("/sys/class/drm/card" + std::to_string(card_index) + "/device/hwmon/hwmon" +
  //                                   std::to_string(hwmon_index) + "/power1_cap_max");

  // if (!std::filesystem::is_regular_file(path)) {
  //   util::debug(log_tag + "file "s + path.string() + " does not exist!");
  //   util::debug(log_tag + "card "s + util::to_string(card_index) + " could not read the maximum power cap!"s);
  // } else {
  //   std::ifstream f;

  //   f.open(path, std::ios::in);

  //   int raw_value = 0;  // microWatts

  //   f >> raw_value;

  //   f.close();

  //   int power_cap_in_watts = raw_value / 1000000;

  //   auto* adj = gtk_spin_button_get_adjustment(self->power_cap0);

  //   gtk_adjustment_set_upper(adj, power_cap_in_watts);

  //   util::debug(log_tag + "card "s + util::to_string(card_index) + " maximum allowed power cap: "s +
  //               std::to_string(power_cap_in_watts) + " W");
  // }
}

void read_power_cap(Nvidia* self) {
  // auto hwmon_index = util::find_hwmon_index(card_index);

  // auto path = std::filesystem::path("/sys/class/drm/card" + std::to_string(card_index) + "/device/hwmon/hwmon" +
  //                                   std::to_string(hwmon_index) + "/power1_cap");

  // if (!std::filesystem::is_regular_file(path)) {
  //   gtk_widget_set_sensitive(GTK_WIDGET(self->power_cap0), 0);

  //   util::debug(log_tag + "file "s + path.string() + " does not exist!");
  //   util::debug(log_tag + "card "s + util::to_string(card_index) + " could not change the power cap!"s);
  // } else {
  //   std::ifstream f;

  //   f.open(path, std::ios::in);

  //   int raw_value = 0;  // microWatts

  //   f >> raw_value;

  //   f.close();

  //   int power_cap_in_watts = raw_value / 1000000;

  //   gtk_spin_button_set_value(self->power_cap0, power_cap_in_watts);

  //   util::debug(log_tag + "card "s + util::to_string(card_index) + " current power cap: "s +
  //               std::to_string(power_cap_in_watts) + " W");
  // }
}

void read_performance_level(Nvidia* self) {
  // auto path = std::filesystem::path("/sys/class/drm/card" + std::to_string(card_index) +
  //                                   "/device/power_dpm_force_performance_level");

  // if (!std::filesystem::is_regular_file(path)) {
  //   util::debug(log_tag + "file "s + path.string() + " does not exist!");
  //   util::debug(log_tag + "card "s + util::to_string(card_index) + " could not read the power profile!"s);
  // } else {
  //   std::ifstream f;

  //   f.open(path, std::ios::in);

  //   std::string level;

  //   f >> level;

  //   f.close();

  //   set_performance_level(self, level);

  //   util::debug(log_tag + "card "s + util::to_string(card_index) + " current performance level: "s + level);
  // }
}

void read_power_profile(Nvidia* self) {
  // auto path =
  //     std::filesystem::path("/sys/class/drm/card" + std::to_string(card_index) + "/device/pp_power_profile_mode");

  // auto dropdown = (card_index == 0) ? self->power_profile0 : self->power_profile1;

  // if (!std::filesystem::is_regular_file(path)) {
  //   gtk_widget_set_sensitive(GTK_WIDGET(dropdown), 0);

  //   util::debug(log_tag + "file "s + path.string() + " does not exist!");
  //   util::debug(log_tag + "card "s + util::to_string(card_index) + " could not change the performance level!"s);
  // } else {
  //   std::ifstream f;

  //   f.open(path, std::ios::in);

  //   std::stringstream buffer;

  //   buffer << f.rdbuf();

  //   f.close();

  //   auto* model = reinterpret_cast<GtkStringList*>(gtk_drop_down_get_model(dropdown));

  //   gtk_string_list_splice(model, 0, g_list_model_get_n_items(G_LIST_MODEL(model)), nullptr);

  //   std::string line;

  //   std::getline(buffer, line);  // Discarding the header

  //   while (std::getline(buffer, line)) {
  //     if (line.ends_with(":")) {
  //       line.pop_back();

  //       std::istringstream ss(line);

  //       uint count = 0;

  //       std::string word;
  //       std::string profile;
  //       std::string id_str;
  //       bool is_selected = false;

  //       while (ss >> word) {
  //         if (word.ends_with("*")) {
  //           word.pop_back();

  //           util::debug(log_tag + "card "s + util::to_string(card_index) + " current power profile: "s + word);

  //           is_selected = true;
  //         }

  //         if (count == 0) {
  //           id_str = word;
  //         }

  //         if (count == 1) {
  //           profile = word;
  //         }

  //         count++;
  //       }

  //       // util::info(id + " -> " + profile);

  //       gtk_string_list_append(model, profile.c_str());

  //       if (is_selected) {
  //         uint id = 0;

  //         util::str_to_num(id_str, id);

  //         gtk_drop_down_set_selected(dropdown, id);
  //       }
  //     }
  //   }
  // }
}

void dispose(GObject* object) {
  util::debug(log_tag + "disposed"s);

  G_OBJECT_CLASS(nvidia_parent_class)->dispose(object);
}

void finalize(GObject* object) {
  util::debug(log_tag + "finalized"s);

  G_OBJECT_CLASS(nvidia_parent_class)->finalize(object);
}

void nvidia_class_init(NvidiaClass* klass) {
  auto* object_class = G_OBJECT_CLASS(klass);
  auto* widget_class = GTK_WIDGET_CLASS(klass);

  object_class->dispose = dispose;
  object_class->finalize = finalize;

  gtk_widget_class_set_template_from_resource(widget_class, "/com/github/wwmm/fastgame/ui/nvidia.ui");

  gtk_widget_class_bind_template_child(widget_class, Nvidia, performance_level0);
  gtk_widget_class_bind_template_child(widget_class, Nvidia, power_profile0);
  gtk_widget_class_bind_template_child(widget_class, Nvidia, power_cap0);
}

void nvidia_init(Nvidia* self) {
  gtk_widget_init_template(GTK_WIDGET(self));

  ui::prepare_spinbutton<"W">(self->power_cap0);

  util::debug(log_tag + "number of nvidia cards: "s + util::to_string(n_cards));

#ifdef USE_NVIDIA
  nv_wrapper = std::make_unique<nvidia_wrapper::Nvidia>();

  // if (nvidia->has_gpu()) {
  //   auto gpu_offset = cfg->get_key("general.nvidia.clock-offset.gpu.default", 0);
  //   auto memory_offset = cfg->get_key("general.nvidia.clock-offset.memory.default", 0);
  //   auto powermizer_mode = cfg->get_key<std::string>("general.nvidia.powermizer-mode.default", "auto");
  //   auto power_limit = cfg->get_key("general.nvidia.power-limit.default", -1);

  //   nvidia->set_powermizer_mode(0, powermizer_mode);
  //   nvidia->set_clock_offset(0, gpu_offset, memory_offset);
  //   nvidia->nvml->set_power_limit(0, power_limit);
  // }
#endif

  g_signal_connect(self->performance_level0, "notify::selected-item",
                   G_CALLBACK(+[](GtkDropDown* dropdown, GParamSpec* pspec, Nvidia* self) {
                     if (auto selected_item = gtk_drop_down_get_selected_item(dropdown); selected_item != nullptr) {
                       auto* level = gtk_string_object_get_string(GTK_STRING_OBJECT(selected_item));

                       if (std::strcmp(level, "manual") == 0) {
                         gtk_widget_set_sensitive(GTK_WIDGET(self->power_profile0), 1);
                       } else {
                         gtk_widget_set_sensitive(GTK_WIDGET(self->power_profile0), 0);
                       }
                     }
                   }),
                   self);

  read_power_cap_max(self);
  read_power_cap(self);
  read_performance_level(self);
  read_power_profile(self);

  if (get_performance_level(self) == "manual") {
    gtk_widget_set_sensitive(GTK_WIDGET(self->power_profile0), 1);
  } else {
    gtk_widget_set_sensitive(GTK_WIDGET(self->power_profile0), 0);
  }
}

auto create() -> Nvidia* {
  return static_cast<Nvidia*>(g_object_new(FG_TYPE_NVIDIA, nullptr));
}

}  // namespace ui::nvidia
