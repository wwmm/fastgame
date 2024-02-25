#pragma once

#include <adwaita.h>
#include <udisks/udisks.h>
#include <filesystem>
#include <fstream>
#include <string>

namespace ui::disk {

G_BEGIN_DECLS

#define FG_TYPE_DISK (disk_get_type())

G_DECLARE_FINAL_TYPE(Disk, disk, FG, DISK, GtkBox)

G_END_DECLS

auto create() -> Disk*;

void set_mounting_path(Disk* self, const std::string& name);

auto get_mounting_path(Disk* self) -> std::string;

void set_scheduler(Disk* self, const std::string& name);

auto get_scheduler(Disk* self) -> std::string;

void set_readahead(Disk* self, const int& value);

auto get_readahead(Disk* self) -> int;

void set_nr_requests(Disk* self, const int& value);

auto get_nr_requests(Disk* self) -> int;

void set_rq_affinity(Disk* self, const int& value);

auto get_rq_affinity(Disk* self) -> int;

void set_nomerges(Disk* self, const int& value);

auto get_nomerges(Disk* self) -> int;

void set_wbt_lat_usec(Disk* self, const int& value);

auto get_wbt_lat_usec(Disk* self) -> int;

void set_enable_realtime_priority(Disk* self, const bool& value);

auto get_enable_realtime_priority(Disk* self) -> bool;

void set_enable_add_random(Disk* self, const bool& value);

auto get_enable_add_random(Disk* self) -> bool;

void set_disable_apm(Disk* self, const bool& value);

auto get_disable_apm(Disk* self) -> bool;

void set_enable_write_cache(Disk* self, const bool& value);

auto get_enable_write_cache(Disk* self) -> bool;

auto get_supports_apm() -> bool;

auto get_supports_write_cache() -> bool;

auto get_drive_id() -> std::string;

}  // namespace ui::disk
