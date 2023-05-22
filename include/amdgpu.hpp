#pragma once

#include <adwaita.h>
#include <filesystem>
#include <fstream>
#include <string>
#include "ui_helpers.hpp"
#include "util.hpp"

namespace ui::amdgpu {

G_BEGIN_DECLS

#define FG_TYPE_AMDGPU (amdgpu_get_type())

G_DECLARE_FINAL_TYPE(Amdgpu, amdgpu, FG, AMDGPU, GtkBox)

G_END_DECLS

auto create() -> Amdgpu*;

auto get_n_cards() -> int;

void set_performance_level(Amdgpu* self, const std::string& name, const int& card_index = 0);

auto get_performance_level(Amdgpu* self, const int& card_index = 0) -> std::string;

void set_power_profile(Amdgpu* self, const std::string& id, const int& card_index = 0);

auto get_power_profile(Amdgpu* self, const int& card_index = 0) -> std::string;

void set_power_cap(Amdgpu* self, const int& value, const int& card_index = 0);

auto get_power_cap(Amdgpu* self, const int& card_index = 0) -> int;

}  // namespace ui::amdgpu
