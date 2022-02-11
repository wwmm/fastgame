#pragma once

#include <adwaita.h>
#include <filesystem>
#include "application.hpp"
#include "environment_variable_holder.hpp"

namespace ui::environment_variables {

G_BEGIN_DECLS

#define FG_TYPE_ENVIRONMENT_VARIABLES (environment_variables_get_type())

G_DECLARE_FINAL_TYPE(EnvironmentVariables, environment_variables, FG, ENVIRONMENT_VARIABLES, GtkBox)

G_END_DECLS

auto create() -> EnvironmentVariables*;

void add_list(const std::vector<std::string>& list);

auto get_list() -> std::vector<std::string>;

}  // namespace ui::environment_variables
