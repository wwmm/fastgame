#pragma once

#include <adwaita.h>
#include <filesystem>
#include <fstream>
#include <string>
#include "config.h"
#include "ui_helpers.hpp"
#include "util.hpp"

#ifdef USE_NVIDIA
#include "nvidia/nvidia.hpp"
#endif

namespace ui::nvidia {

G_BEGIN_DECLS

#define FG_TYPE_NVIDIA (nvidia_get_type())

G_DECLARE_FINAL_TYPE(Nvidia, nvidia, FG, NVIDIA, GtkBox)

G_END_DECLS

auto create() -> Nvidia*;

auto has_gpu() -> bool;

void set_powermize_mode(Nvidia* self, const int& mode_id, const int& card_index = 0);

auto get_powermize_mode(Nvidia* self, const int& card_index = 0) -> int;

void set_gpu_clock_offset(Nvidia* self, const int& value, const int& card_index = 0);

auto get_gpu_clock_offset(Nvidia* self, const int& card_index = 0) -> int;

void set_memory_clock_offset(Nvidia* self, const int& value, const int& card_index = 0);

auto get_memory_clock_offset(Nvidia* self, const int& card_index = 0) -> int;

}  // namespace ui::nvidia