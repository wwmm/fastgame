#pragma once

#include <adwaita.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

namespace ui::amdgpu {

G_BEGIN_DECLS

#define FG_TYPE_AMDGPU (amdgpu_get_type())

G_DECLARE_FINAL_TYPE(Amdgpu, amdgpu, FG, AMDGPU, GtkBox)

G_END_DECLS

auto create() -> Amdgpu*;

auto get_n_cards() -> int;

auto get_card_indices() -> std::vector<int>;

void set_performance_level(Amdgpu* self, const std::string& name, const int& card_index);

auto get_performance_level(Amdgpu* self, const int& card_index) -> std::string;

void set_power_profile(Amdgpu* self, const int& id, const int& card_index);

auto get_power_profile(Amdgpu* self, const int& card_index) -> int;

void set_power_cap(Amdgpu* self, const int& value, const int& card_index);

auto get_power_cap(Amdgpu* self, const int& card_index) -> int;

}  // namespace ui::amdgpu
