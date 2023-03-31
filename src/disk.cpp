#include "disk.hpp"

namespace ui::disk {

using namespace std::string_literals;

auto constexpr log_tag = "disk: ";

UDisksClient* udisks_client = nullptr;

bool supports_apm = false, supports_write_cache = false;

std::string drive_id;

struct _Disk {
  GtkPopover parent_instance;

  GtkDropDown *device, *scheduler;

  GtkSpinButton *readahead, *nr_requests, *rq_affinity, *nomerges;

  GtkSwitch *enable_write_cache, *disable_apm, *add_random, *enable_realtime_priority;
};

G_DEFINE_TYPE(Disk, disk, GTK_TYPE_BOX)

void set_device(Disk* self, const std::string& name) {
  auto* model = reinterpret_cast<GtkStringList*>(gtk_drop_down_get_model(self->device));

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

  gtk_drop_down_set_selected(self->device, id);
}

auto get_device(Disk* self) -> std::string {
  auto* selected_item = gtk_drop_down_get_selected_item(self->device);

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
    auto selected_device = active_text.substr(11);

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

  gtk_widget_class_bind_template_child(widget_class, Disk, device);
  gtk_widget_class_bind_template_child(widget_class, Disk, scheduler);
  gtk_widget_class_bind_template_child(widget_class, Disk, readahead);
  gtk_widget_class_bind_template_child(widget_class, Disk, nr_requests);
  gtk_widget_class_bind_template_child(widget_class, Disk, rq_affinity);
  gtk_widget_class_bind_template_child(widget_class, Disk, nomerges);
  gtk_widget_class_bind_template_child(widget_class, Disk, enable_write_cache);
  gtk_widget_class_bind_template_child(widget_class, Disk, disable_apm);
  gtk_widget_class_bind_template_child(widget_class, Disk, add_random);
  gtk_widget_class_bind_template_child(widget_class, Disk, enable_realtime_priority);
}

void disk_init(Disk* self) {
  gtk_widget_init_template(GTK_WIDGET(self));

  ui::prepare_spinbutton<"KB">(self->readahead);

  GError* error = nullptr;

  udisks_client = udisks_client_new_sync(nullptr, &error);

  if (udisks_client == nullptr) {
    util::warning(log_tag + "Error connecting to the udisks daemon: "s + error->message);

    g_error_free(error);
  }

  g_signal_connect(
      self->device, "notify::selected-item", G_CALLBACK(+[](GtkDropDown* dropdown, GParamSpec* pspec, Disk* self) {
        if (auto selected_item = gtk_drop_down_get_selected_item(dropdown); selected_item != nullptr) {
          auto* device_path = gtk_string_object_get_string(GTK_STRING_OBJECT(selected_item));

          init_scheduler(self, device_path);
          init_udisks_object(self, device_path);

          gtk_spin_button_set_value(self->nr_requests,
                                    std::stoi(util::read_system_setting(device_path + "/queue/nr_requests"s)[0]));

          gtk_spin_button_set_value(self->rq_affinity,
                                    std::stoi(util::read_system_setting(device_path + "/queue/rq_affinity"s)[0]));

          gtk_spin_button_set_value(self->nomerges,
                                    std::stoi(util::read_system_setting(device_path + "/queue/nomerges"s)[0]));

          gtk_spin_button_set_value(self->readahead,
                                    std::stoi(util::read_system_setting(device_path + "/queue/read_ahead_kb"s)[0]));

          gtk_switch_set_active(
              self->add_random,
              static_cast<bool>(std::stoi(util::read_system_setting(device_path + "/queue/add_random"s)[0])));
        }
      }),
      self);

  auto* model = reinterpret_cast<GtkStringList*>(gtk_drop_down_get_model(self->device));

  gtk_string_list_splice(model, 0, g_list_model_get_n_items(G_LIST_MODEL(model)), nullptr);

  for (const auto& entry : std::filesystem::directory_iterator("/sys/block")) {
    auto path = entry.path().string();

    gtk_string_list_append(model, path.c_str());
  }

  gtk_drop_down_set_selected(self->device, 0);
}

auto create() -> Disk* {
  return static_cast<Disk*>(g_object_new(FG_TYPE_DISK, nullptr));
}

}  // namespace ui::disk
