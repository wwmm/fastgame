#include "disk.hpp"

namespace ui::disk {

using namespace std::string_literals;

auto constexpr log_tag = "disk: ";

struct _Disk {
  GtkPopover parent_instance;

  GtkComboBoxText *device, *scheduler;

  GtkSpinButton *readahead, *nr_requests;

  GtkSwitch *enable_write_cache, *disable_apm, *add_random, *enable_realtime_priority;
};

G_DEFINE_TYPE(Disk, disk, GTK_TYPE_BOX)

// void set_performance_level(Disk* self, const std::string& name) {
//   gtk_combo_box_set_active_id(GTK_COMBO_BOX(self->performance_level), name.c_str());
// }

// auto get_performance_level(Disk* self) -> std::string {
//   return gtk_combo_box_text_get_active_text(self->performance_level);
// }

// void set_power_cap(Disk* self, const int& value) {
//   gtk_spin_button_set_value(self->power_cap, value);
// }

// auto get_power_cap(Disk* self) -> int {
//   return static_cast<int>(gtk_spin_button_get_value(self->power_cap));
// }

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
}

void disk_init(Disk* self) {
  gtk_widget_init_template(GTK_WIDGET(self));

  ui::prepare_spinbutton<"KB">(self->readahead);
}

auto create() -> Disk* {
  return static_cast<Disk*>(g_object_new(FG_TYPE_DISK, nullptr));
}

}  // namespace ui::disk

// Disk::Disk(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder) : Gtk::Grid(cobject) {
//   // loading glade widgets

//   builder->get_widget("device", device);
//   builder->get_widget("scheduler", scheduler);
//   builder->get_widget("enable_realtime_priority", enable_realtime_priority);
//   builder->get_widget("add_random", add_random);
//   builder->get_widget("disable_apm", disable_apm);
//   builder->get_widget("enable_write_cache", enable_write_cache);

//   readahead = Glib::RefPtr<Gtk::Adjustment>::cast_dynamic(builder->get_object("readahead"));
//   nr_requests = Glib::RefPtr<Gtk::Adjustment>::cast_dynamic(builder->get_object("nr_requests"));

//   // initializing the udisk client

//   GError* error = nullptr;

//   udisks_client = udisks_client_new_sync(nullptr, &error);

//   if (udisks_client == nullptr) {
//     util::warning(log_tag + "Error connecting to the udisks daemon: " + error->message);

//     g_error_free(error);
//   }

//   // initializing widgets

//   for (const auto& entry : std::filesystem::directory_iterator("/sys/block")) {
//     device->append(entry.path().string());
//   }

//   device->signal_changed().connect([=]() {
//     init_scheduler();

//     init_udisks_object();

//     nr_requests->set_value(std::stoi(util::read_system_setting(device->get_active_text() +
//     "/queue/nr_requests")[0]));

//     readahead->set_value(std::stoi(util::read_system_setting(device->get_active_text() +
//     "/queue/read_ahead_kb")[0]));

//     add_random->set_active(
//         static_cast<bool>(std::stoi(util::read_system_setting(device->get_active_text() + "/queue/add_random")[0])));
//   });

//   device->set_active(0);
// }

// Disk::~Disk() {
//   util::debug(log_tag + "destroyed");
// }

// auto Disk::add_to_stack(Gtk::Stack* stack) -> Disk* {
//   auto builder = Gtk::Builder::create_from_resource("/com/github/wwmm/fastgame/ui/disk.glade");

//   Disk* ui = nullptr;

//   builder->get_widget_derived("widgets_grid", ui);

//   stack->add(*ui, "disk", _("Storage Device"));

//   return ui;
// }

// void Disk::init_scheduler() {
//   auto scheduler_list = util::read_system_setting(device->get_active_text() + "/queue/scheduler");

//   auto scheduler_value = util::get_selected_value(scheduler_list);

//   scheduler->remove_all();

//   for (auto& value : scheduler_list) {
//     if (value.find('[') != std::string::npos) {
//       value = value.erase(0, 1).erase(value.size() - 1, 1);  // removing the [] characters
//     }

//     scheduler->append(value);
//   }

//   scheduler->set_active_text(scheduler_value);
// }

// void Disk::init_udisks_object() {
//   supports_apm = false;

//   drive_id.clear();

//   disable_apm->set_sensitive(false);
//   enable_write_cache->set_sensitive(false);

//   if (udisks_client != nullptr) {
//     auto selected_device = device->get_active_text().substr(11);

//     auto* objects = g_dbus_object_manager_get_objects(udisks_client_get_object_manager(udisks_client));

//     while (objects != nullptr) {
//       auto* object = UDISKS_OBJECT(objects->data);

//       auto* drive = udisks_object_get_drive(object);

//       if (drive != nullptr) {
//         auto* block = udisks_client_get_block_for_drive(udisks_client, drive, 1 /* get physical*/);

//         if (block != nullptr) {
//           auto device = std::string(udisks_block_get_device(block)).substr(5);

//           if (device == selected_device) {
//             auto* drive_ata = udisks_object_get_drive_ata(object);

//             if (drive_ata != nullptr) {
//               supports_apm = udisks_drive_ata_get_apm_supported(drive_ata) == 1;
//               supports_write_cache = udisks_drive_ata_get_write_cache_supported(drive_ata) == 1;

//               disable_apm->set_sensitive(supports_apm);
//               enable_write_cache->set_sensitive(supports_write_cache);

//               drive_id = udisks_drive_get_id(drive);

//               // reading the current configuration

//               auto* config = udisks_drive_get_configuration(drive);

//               GVariantIter iter;
//               GVariant* child = nullptr;
//               gchar* key = nullptr;

//               g_variant_iter_init(&iter, config);

//               while (g_variant_iter_next(&iter, "{sv}", &key, &child) != 0) {
//                 if (std::strcmp(key, "ata-apm-level") == 0) {
//                   if (g_variant_type_equal(g_variant_get_type(child), G_VARIANT_TYPE_INT32) != 0) {
//                     disable_apm->set_active(g_variant_get_int32(child) == 255);
//                   }
//                 }

//                 if (std::strcmp(key, "ata-write-cache-enabled") == 0) {
//                   if (g_variant_type_equal(g_variant_get_type(child), G_VARIANT_TYPE_BOOLEAN) != 0) {
//                     enable_write_cache->set_active(g_variant_get_boolean(child) != 0);
//                   }
//                 }
//               }

//             } else {
//               util::debug(log_tag + "device " + selected_device + " does not support the ata interface");
//             }

//             break;
//           }
//         }
//       }

//       objects = objects->next;
//     }
//   }
// }

// auto Disk::get_device() -> std::string {
//   return device->get_active_text();
// }

// void Disk::set_device(const std::string& value) {
//   device->set_active_text(value);
// }

// auto Disk::get_scheduler() -> std::string {
//   return scheduler->get_active_text();
// }

// void Disk::set_scheduler(const std::string& value) {
//   scheduler->set_active_text(value);
// }

// auto Disk::get_readahead() -> int {
//   return static_cast<int>(readahead->get_value());
// }

// void Disk::set_readahead(const int& value) {
//   readahead->set_value(value);
// }

// auto Disk::get_nr_requests() -> int {
//   return static_cast<int>(nr_requests->get_value());
// }

// void Disk::set_nr_requests(const int& value) {
//   nr_requests->set_value(value);
// }

// auto Disk::get_enable_realtime_priority() -> bool {
//   return enable_realtime_priority->get_active();
// }

// void Disk::set_enable_realtime_priority(const bool& value) {
//   enable_realtime_priority->set_active(value);
// }

// auto Disk::get_enable_add_random() -> bool {
//   return add_random->get_active();
// }

// void Disk::set_enable_add_random(const bool& value) {
//   add_random->set_active(value);
// }

// auto Disk::get_drive_id() -> std::string {
//   return drive_id;
// }

// auto Disk::get_disable_apm() -> bool {
//   return disable_apm->get_active();
// }

// void Disk::set_disable_apm(const bool& value) {
//   disable_apm->set_active(value);
// }

// auto Disk::get_supports_apm() const -> bool {
//   return supports_apm;
// }

// auto Disk::get_enable_write_cache() -> bool {
//   return enable_write_cache->get_active();
// }

// void Disk::set_enable_write_cache(const bool& value) {
//   enable_write_cache->set_active(value);
// }

// auto Disk::get_supports_write_cache() const -> bool {
//   return supports_write_cache;
// }
