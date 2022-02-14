#pragma once

#include <adwaita.h>
#include <udisks/udisks.h>
#include <filesystem>
#include <fstream>
#include <string>
#include "ui_helpers.hpp"
#include "util.hpp"

namespace ui::disk {

G_BEGIN_DECLS

#define FG_TYPE_DISK (disk_get_type())

G_DECLARE_FINAL_TYPE(Disk, disk, FG, DISK, GtkBox)

G_END_DECLS

auto create() -> Disk*;

}  // namespace ui::disk

// auto get_device() -> std::string;

// void set_device(const std::string& value);

// auto get_scheduler() -> std::string;

// void set_scheduler(const std::string& value);

// auto get_readahead() -> int;

// void set_readahead(const int& value);

// auto get_nr_requests() -> int;

// void set_nr_requests(const int& value);

// auto get_enable_realtime_priority() -> bool;

// void set_enable_realtime_priority(const bool& value);

// auto get_enable_add_random() -> bool;

// void set_enable_add_random(const bool& value);

// auto get_drive_id() -> std::string;

// auto get_disable_apm() -> bool;

// void set_disable_apm(const bool& value);

// auto get_supports_apm() const -> bool;

// auto get_enable_write_cache() -> bool;

// void set_enable_write_cache(const bool& value);

// auto get_supports_write_cache() const -> bool;

//  private:
//

//   void init_scheduler();

//   void init_udisks_object();
// };

// #endif
