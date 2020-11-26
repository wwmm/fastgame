#include "disk.hpp"
#include <glibmm/i18n.h>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <string>
#include "util.hpp"

namespace fs = std::filesystem;

Disk::Disk(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder) : Gtk::Grid(cobject) {
  // loading glade widgets

  builder->get_widget("device", device);
  builder->get_widget("scheduler", scheduler);
  builder->get_widget("enable_realtime_priority", enable_realtime_priority);
  builder->get_widget("add_random", add_random);
  builder->get_widget("disable_apm", disable_apm);

  readahead = Glib::RefPtr<Gtk::Adjustment>::cast_dynamic(builder->get_object("readahead"));
  nr_requests = Glib::RefPtr<Gtk::Adjustment>::cast_dynamic(builder->get_object("nr_requests"));

  // initializing the udisk client

  GError* error = nullptr;

  udisks_client = udisks_client_new_sync(nullptr, &error);

  if (udisks_client == nullptr) {
    util::warning(log_tag + "Error connecting to the udisks daemon: " + error->message);

    g_error_free(error);
  }

  // initializing widgets

  for (const auto& entry : std::filesystem::directory_iterator("/sys/block")) {
    device->append(entry.path().string());
  }

  device->signal_changed().connect([=]() {
    init_scheduler();

    init_udisks_object();

    nr_requests->set_value(std::stoi(util::read_system_setting(device->get_active_text() + "/queue/nr_requests")[0]));

    readahead->set_value(std::stoi(util::read_system_setting(device->get_active_text() + "/queue/read_ahead_kb")[0]));

    add_random->set_active(
        static_cast<bool>(std::stoi(util::read_system_setting(device->get_active_text() + "/queue/add_random")[0])));
  });

  device->set_active(0);
}

Disk::~Disk() {
  util::debug(log_tag + "destroyed");
}

auto Disk::add_to_stack(Gtk::Stack* stack) -> Disk* {
  auto builder = Gtk::Builder::create_from_resource("/com/github/wwmm/fastgame/ui/disk.glade");

  Disk* ui = nullptr;

  builder->get_widget_derived("widgets_grid", ui);

  stack->add(*ui, "disk", _("Storage Device"));

  return ui;
}

void Disk::init_scheduler() {
  auto scheduler_list = util::read_system_setting(device->get_active_text() + "/queue/scheduler");

  auto scheduler_value = util::get_selected_value(scheduler_list);

  scheduler->remove_all();

  for (auto& value : scheduler_list) {
    if (value.find('[') != std::string::npos) {
      value = value.erase(0, 1).erase(value.size() - 1, 1);  // removing the [] characters
    }

    scheduler->append(value);
  }

  scheduler->set_active_text(scheduler_value);
}

void Disk::init_udisks_object() {
  apm_level = 127;
  supports_apm = false;

  drive_id.clear();

  disable_apm->set_sensitive(false);

  if (udisks_client != nullptr) {
    auto selected_device = device->get_active_text().substr(11);

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

              drive_id = udisks_drive_get_id(drive);

              if (supports_apm) {
                disable_apm->set_sensitive();

                // reading the current configuration

                auto* config = udisks_drive_get_configuration(drive);

                GVariantIter iter;
                GVariant* child = nullptr;
                gchar* key = nullptr;

                g_variant_iter_init(&iter, config);

                while (g_variant_iter_next(&iter, "{sv}", &key, &child) != 0) {
                  if (std::strcmp(key, "ata-apm-level") == 0) {
                    if (g_variant_type_equal(g_variant_get_type(child), G_VARIANT_TYPE_INT32) != 0) {
                      apm_level = g_variant_get_int32(child);

                      util::warning(drive_id + " apm level: " + std::to_string(apm_level));
                    }
                  }
                }
              } else {
                util::debug(log_tag + "device " + selected_device + " does not support apm control");
              }
            } else {
              util::debug(log_tag + "device " + selected_device + " does not support the ata interface");
            }

            break;
          }
        }
      }

      objects = objects->next;
    }
  }

  disable_apm->set_active(apm_level == 255);
}

auto Disk::get_device() -> std::string {
  return device->get_active_text();
}

void Disk::set_device(const std::string& value) {
  device->set_active_text(value);
}

auto Disk::get_scheduler() -> std::string {
  return scheduler->get_active_text();
}

void Disk::set_scheduler(const std::string& value) {
  scheduler->set_active_text(value);
}

auto Disk::get_readahead() -> int {
  return static_cast<int>(readahead->get_value());
}

void Disk::set_readahead(const int& value) {
  readahead->set_value(value);
}

auto Disk::get_nr_requests() -> int {
  return static_cast<int>(nr_requests->get_value());
}

void Disk::set_nr_requests(const int& value) {
  nr_requests->set_value(value);
}

auto Disk::get_enable_realtime_priority() -> bool {
  return enable_realtime_priority->get_active();
}

void Disk::set_enable_realtime_priority(const bool& value) {
  enable_realtime_priority->set_active(value);
}

auto Disk::get_enable_add_random() -> bool {
  return add_random->get_active();
}

void Disk::set_enable_add_random(const bool& value) {
  add_random->set_active(value);
}

auto Disk::get_drive_id() -> std::string {
  return drive_id;
}

auto Disk::get_disable_apm() -> bool {
  return disable_apm->get_active();
}

void Disk::set_disable_apm(const bool& value) {
  disable_apm->set_active(value);
}

auto Disk::get_supports_apm() const -> bool {
  return supports_apm;
}
