#pragma once

#include <adwaita.h>

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

void set_power_limit(Nvidia* self, const int& value, const int& card_index = 0);

auto get_power_limit(Nvidia* self, const int& card_index = 0) -> int;

}  // namespace ui::nvidia
