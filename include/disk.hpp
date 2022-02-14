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

void set_device(Disk* self, const std::string& name);

auto get_device(Disk* self) -> std::string;

void set_scheduler(Disk* self, const std::string& name);

auto get_scheduler(Disk* self) -> std::string;

void set_readahead(Disk* self, const int& value);

auto get_readahead(Disk* self) -> int;

void set_nr_requests(Disk* self, const int& value);

auto get_nr_requests(Disk* self) -> int;

}  // namespace ui::disk
