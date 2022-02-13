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

auto get_card_index(Amdgpu* self) -> int;

void set_performance_level(Amdgpu* self, const std::string& name);

auto get_performance_level(Amdgpu* self) -> std::string;

void set_power_cap(Amdgpu* self, const int& value);

auto get_power_cap(Amdgpu* self) -> int;

}  // namespace ui::amdgpu
