#include "disk.hpp"

namespace ui::disk {

using namespace std::string_literals;

auto constexpr log_tag = "disk: ";

UDisksClient* udisks_client = nullptr;

bool supports_apm = false, supports_write_cache = false;

std::string drive_id;

struct _Disk {
  GtkPopover parent_instance;

  GtkComboBoxText *device, *scheduler;

  GtkSpinButton *readahead, *nr_requests;

  GtkSwitch *enable_write_cache, *disable_apm, *add_random, *enable_realtime_priority;
};

G_DEFINE_TYPE(Disk, disk, GTK_TYPE_BOX)

void set_device(Disk* self, const std::string& name) {
  gtk_combo_box_set_active_id(GTK_COMBO_BOX(self->device), name.c_str());
}

auto get_device(Disk* self) -> std::string {
  return gtk_combo_box_text_get_active_text(self->device);
}

void set_scheduler(Disk* self, const std::string& name) {
  gtk_combo_box_set_active_id(GTK_COMBO_BOX(self->scheduler), name.c_str());
}

auto get_scheduler(Disk* self) -> std::string {
  return gtk_combo_box_text_get_active_text(self->scheduler);
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

void set_enable_disable_apm(Disk* self, const bool& value) {
  gtk_switch_set_active(self->disable_apm, value);
}

auto get_enable_disable_apm(Disk* self) -> bool {
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

  gtk_combo_box_text_remove_all(self->scheduler);

  for (auto& value : scheduler_list) {
    if (value.find('[') != std::string::npos) {
      value = value.erase(0, 1).erase(value.size() - 1, 1);  // removing the [] characters
    }

    gtk_combo_box_text_append(self->scheduler, value.c_str(), value.c_str());
  }

  gtk_combo_box_set_active_id(GTK_COMBO_BOX(self->scheduler), scheduler_value.c_str());
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

void on_device_changed(GtkComboBox* combo, Disk* self) {
  auto active_text = gtk_combo_box_text_get_active_text(self->device);

  init_scheduler(self, active_text);
  init_udisks_object(self, active_text);

  gtk_spin_button_set_value(self->nr_requests,
                            std::stoi(util::read_system_setting(active_text + "/queue/nr_requests"s)[0]));

  gtk_spin_button_set_value(self->readahead,
                            std::stoi(util::read_system_setting(active_text + "/queue/read_ahead_kb"s)[0]));

  gtk_switch_set_active(self->add_random,
                        static_cast<bool>(std::stoi(util::read_system_setting(active_text + "/queue/add_random"s)[0])));
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
  gtk_widget_class_bind_template_child(widget_class, Disk, enable_write_cache);
  gtk_widget_class_bind_template_child(widget_class, Disk, disable_apm);
  gtk_widget_class_bind_template_child(widget_class, Disk, add_random);
  gtk_widget_class_bind_template_child(widget_class, Disk, enable_realtime_priority);

  gtk_widget_class_bind_template_callback(widget_class, on_device_changed);
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

  for (const auto& entry : std::filesystem::directory_iterator("/sys/block")) {
    auto path = entry.path().string();

    gtk_combo_box_text_append(self->device, path.c_str(), path.c_str());
  }

  gtk_combo_box_set_active(GTK_COMBO_BOX(self->device), 0);
}

auto create() -> Disk* {
  return static_cast<Disk*>(g_object_new(FG_TYPE_DISK, nullptr));
}

}  // namespace ui::disk
