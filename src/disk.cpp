#include "disk.hpp"
#include "ui_helpers.hpp"
#include "util.hpp"

namespace ui::disk {

using namespace std::string_literals;

auto constexpr log_tag = "disk: ";

UDisksClient* udisks_client = nullptr;

bool supports_apm = false, supports_write_cache = false;

std::string drive_id;

struct _Disk {
  GtkPopover parent_instance;

  GtkDropDown *mounting_path, *scheduler;

  GtkSpinButton *readahead, *nr_requests, *rq_affinity, *nomerges, *wbt_lat_usec;

  GtkSwitch *enable_write_cache, *disable_apm, *add_random, *enable_realtime_priority;
};

G_DEFINE_TYPE(Disk, disk, GTK_TYPE_BOX)

void set_mounting_path(Disk* self, const std::string& name) {
  auto* model = reinterpret_cast<GtkStringList*>(gtk_drop_down_get_model(self->mounting_path));

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

  gtk_drop_down_set_selected(self->mounting_path, id);
}

auto get_mounting_path(Disk* self) -> std::string {
  auto* selected_item = gtk_drop_down_get_selected_item(self->mounting_path);

  if (selected_item == nullptr) {
    return "";
  }

  return gtk_string_object_get_string(GTK_STRING_OBJECT(selected_item));
}

void set_scheduler(Disk* self, const std::string& name) {
  auto* model = reinterpret_cast<GtkStringList*>(gtk_drop_down_get_model(self->scheduler));

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

  gtk_drop_down_set_selected(self->scheduler, id);
}

auto get_scheduler(Disk* self) -> std::string {
  auto* selected_item = gtk_drop_down_get_selected_item(self->scheduler);

  if (selected_item == nullptr) {
    return "";
  }

  return gtk_string_object_get_string(GTK_STRING_OBJECT(selected_item));
}

void set_readahead(Disk* self, const int& value) {
  gtk_spin_button_set_value(self->readahead, value);
}

auto get_readahead(Disk* self) -> int {
  return static_cast<int>(gtk_spin_button_get_value(self->readahead));
}

void set_nr_requests(Disk* self, const int& value) {
  gtk_spin_button_set_value(self->nr_requests, value);
}

auto get_nr_requests(Disk* self) -> int {
  return static_cast<int>(gtk_spin_button_get_value(self->nr_requests));
}

void set_rq_affinity(Disk* self, const int& value) {
  gtk_spin_button_set_value(self->rq_affinity, value);
}

auto get_rq_affinity(Disk* self) -> int {
  return static_cast<int>(gtk_spin_button_get_value(self->rq_affinity));
}

void set_nomerges(Disk* self, const int& value) {
  gtk_spin_button_set_value(self->nomerges, value);
}

auto get_nomerges(Disk* self) -> int {
  return static_cast<int>(gtk_spin_button_get_value(self->nomerges));
}

void set_wbt_lat_usec(Disk* self, const int& value) {
  gtk_spin_button_set_value(self->wbt_lat_usec, value);
}

auto get_wbt_lat_usec(Disk* self) -> int {
  return static_cast<int>(gtk_spin_button_get_value(self->wbt_lat_usec));
}

void set_enable_realtime_priority(Disk* self, const bool& value) {
  gtk_switch_set_active(self->enable_realtime_priority, value);
}

auto get_enable_realtime_priority(Disk* self) -> bool {
  return gtk_switch_get_active(self->enable_realtime_priority);
}

void set_enable_add_random(Disk* self, const bool& value) {
  gtk_switch_set_active(self->add_random, value);
}

auto get_enable_add_random(Disk* self) -> bool {
  return gtk_switch_get_active(self->add_random);
}

void set_disable_apm(Disk* self, const bool& value) {
  gtk_switch_set_active(self->disable_apm, value);
}

auto get_disable_apm(Disk* self) -> bool {
  return gtk_switch_get_active(self->disable_apm);
}

void set_enable_write_cache(Disk* self, const bool& value) {
  gtk_switch_set_active(self->enable_write_cache, value);
}

auto get_enable_write_cache(Disk* self) -> bool {
  return gtk_switch_get_active(self->enable_write_cache);
}

auto get_supports_apm() -> bool {
  return supports_apm;
}

auto get_supports_write_cache() -> bool {
  return supports_write_cache;
}

auto get_drive_id() -> std::string {
  return drive_id;
}

void init_scheduler(Disk* self, const std::string& active_text) {
  auto scheduler_list = util::read_system_setting(active_text + "/queue/scheduler");

  auto scheduler_value = util::get_selected_value(scheduler_list);

  auto* model = reinterpret_cast<GtkStringList*>(gtk_drop_down_get_model(self->scheduler));

  gtk_string_list_splice(model, 0, g_list_model_get_n_items(G_LIST_MODEL(model)), nullptr);

  guint selected_id = 0;

  for (size_t n = 0; n < scheduler_list.size(); n++) {
    auto value = scheduler_list[n];

    if (value.find('[') != std::string::npos) {
      value = value.erase(0, 1).erase(value.size() - 1, 1);  // removing the [] characters
    }

    gtk_string_list_append(model, value.c_str());

    if (value == scheduler_value) {
      selected_id = n;
    }
  }

  gtk_drop_down_set_selected(self->scheduler, selected_id);
}

void init_udisks_object(Disk* self, const std::string& active_text) {
  supports_apm = false;

  drive_id.clear();

  gtk_widget_set_sensitive(GTK_WIDGET(self->disable_apm), 0);
  gtk_widget_set_sensitive(GTK_WIDGET(self->enable_write_cache), 0);

  if (udisks_client != nullptr) {
    auto selected_device = active_text.substr(17);

    auto* objects = g_dbus_object_manager_get_objects(udisks_client_get_object_manager(udisks_client));

    while (objects != nullptr) {
      auto* object = UDISKS_OBJECT(objects->data);

      auto* drive = udisks_object_get_drive(object);

      if (drive != nullptr) {
        auto* block = udisks_client_get_block_for_drive(udisks_client, drive, 1 /* get physical*/);

        if (block != nullptr) {
          auto device = std::string(udisks_block_get_device(block)).substr(5);

          if (device == selected_device) {
            auto* drive_ata = udisks_object_get_drive_ata(object);

            if (drive_ata != nullptr) {
              supports_apm = udisks_drive_ata_get_apm_supported(drive_ata) == 1;
              supports_write_cache = udisks_drive_ata_get_write_cache_supported(drive_ata) == 1;

              gtk_widget_set_sensitive(GTK_WIDGET(self->disable_apm), supports_apm);
              gtk_widget_set_sensitive(GTK_WIDGET(self->enable_write_cache), supports_write_cache);

              drive_id = udisks_drive_get_id(drive);

              // reading the current configuration

              auto* config = udisks_drive_get_configuration(drive);

              GVariantIter iter;
              GVariant* child = nullptr;
              gchar* key = nullptr;

              g_variant_iter_init(&iter, config);

              while (g_variant_iter_next(&iter, "{sv}", &key, &child) != 0) {
                if (std::strcmp(key, "ata-apm-level") == 0) {
                  if (g_variant_type_equal(g_variant_get_type(child), G_VARIANT_TYPE_INT32) != 0) {
                    gtk_switch_set_active(self->disable_apm, g_variant_get_int32(child) == 255);
                  }
                }

                if (std::strcmp(key, "ata-write-cache-enabled") == 0) {
                  if (g_variant_type_equal(g_variant_get_type(child), G_VARIANT_TYPE_BOOLEAN) != 0) {
                    gtk_switch_set_active(self->enable_write_cache, g_variant_get_boolean(child) != 0);
                  }
                }
              }

            } else {
              util::debug(log_tag + "device "s + selected_device + " does not support the ata interface");
            }

            break;
          }
        }
      }

      objects = objects->next;
    }
  }
}

void dispose(GObject* object) {
  util::debug(log_tag + "disposed"s);

  G_OBJECT_CLASS(disk_parent_class)->dispose(object);
}

void finalize(GObject* object) {
  util::debug(log_tag + "finalized"s);

  G_OBJECT_CLASS(disk_parent_class)->finalize(object);
}

void disk_class_init(DiskClass* klass) {
  auto* object_class = G_OBJECT_CLASS(klass);
  auto* widget_class = GTK_WIDGET_CLASS(klass);

  object_class->dispose = dispose;
  object_class->finalize = finalize;

  gtk_widget_class_set_template_from_resource(widget_class, "/com/github/wwmm/fastgame/ui/disk.ui");

  gtk_widget_class_bind_template_child(widget_class, Disk, mounting_path);
  gtk_widget_class_bind_template_child(widget_class, Disk, scheduler);
  gtk_widget_class_bind_template_child(widget_class, Disk, readahead);
  gtk_widget_class_bind_template_child(widget_class, Disk, nr_requests);
  gtk_widget_class_bind_template_child(widget_class, Disk, rq_affinity);
  gtk_widget_class_bind_template_child(widget_class, Disk, nomerges);
  gtk_widget_class_bind_template_child(widget_class, Disk, wbt_lat_usec);
  gtk_widget_class_bind_template_child(widget_class, Disk, enable_write_cache);
  gtk_widget_class_bind_template_child(widget_class, Disk, disable_apm);
  gtk_widget_class_bind_template_child(widget_class, Disk, add_random);
  gtk_widget_class_bind_template_child(widget_class, Disk, enable_realtime_priority);
}

void disk_init(Disk* self) {
  gtk_widget_init_template(GTK_WIDGET(self));

  ui::prepare_spinbutton<"KB">(self->readahead);
  ui::prepare_spinbutton<"us">(self->wbt_lat_usec);

  GError* error = nullptr;

  udisks_client = udisks_client_new_sync(nullptr, &error);

  if (udisks_client == nullptr) {
    util::warning(log_tag + "Error connecting to the udisks daemon: "s + error->message);

    g_error_free(error);
  }

  g_signal_connect(
      self->mounting_path, "notify::selected-item",
      G_CALLBACK(+[](GtkDropDown* dropdown, [[maybe_unused]] GParamSpec* pspec, Disk* self) {
        if (auto selected_item = gtk_drop_down_get_selected_item(dropdown); selected_item != nullptr) {
          auto* mounting_path = gtk_string_object_get_string(GTK_STRING_OBJECT(selected_item));
          auto device_path = util::mounting_path_to_sys_class_path(mounting_path);

          init_scheduler(self, device_path);
          init_udisks_object(self, device_path);

          if (const auto list = util::read_system_setting(device_path + "/queue/nr_requests"s); !list.empty()) {
            gtk_spin_button_set_value(self->nr_requests, std::stoi(list[0]));
          }

          if (const auto list = util::read_system_setting(device_path + "/queue/rq_affinity"s); !list.empty()) {
            gtk_spin_button_set_value(self->rq_affinity, std::stoi(list[0]));
          }

          if (const auto list = util::read_system_setting(device_path + "/queue/nomerges"s); !list.empty()) {
            gtk_spin_button_set_value(self->nomerges, std::stoi(list[0]));
          }

          if (const auto list = util::read_system_setting(device_path + "/queue/read_ahead_kb"s); !list.empty()) {
            gtk_spin_button_set_value(self->readahead, std::stoi(list[0]));
          }

          if (const auto list = util::read_system_setting(device_path + "/queue/wbt_lat_usec"s); !list.empty()) {
            gtk_spin_button_set_value(self->wbt_lat_usec, std::stoi(list[0]));
          }

          gtk_switch_set_active(
              self->add_random,
              static_cast<bool>(std::stoi(util::read_system_setting(device_path + "/queue/add_random"s)[0])));
        }
      }),
      self);

  auto* model = reinterpret_cast<GtkStringList*>(gtk_drop_down_get_model(self->mounting_path));

  gtk_string_list_splice(model, 0, g_list_model_get_n_items(G_LIST_MODEL(model)), nullptr);

  std::ifstream infile("/proc/self/mounts");

  std::string psm_dev_path, mounting_path;

  while (infile >> psm_dev_path >> mounting_path) {
    if (psm_dev_path.starts_with("/dev/")) {
      gtk_string_list_append(model, mounting_path.c_str());
    }
  }

  gtk_drop_down_set_selected(self->mounting_path, 0);
}

auto create() -> Disk* {
  return static_cast<Disk*>(g_object_new(FG_TYPE_DISK, nullptr));
}

}  // namespace ui::disk
