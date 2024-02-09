#pragma once

#include <adwaita.h>
#include <filesystem>
#include <vector>

namespace ui::command_line_arguments {

G_BEGIN_DECLS

#define FG_TYPE_COMMAND_LINE_ARGUMENTS (command_line_arguments_get_type())

G_DECLARE_FINAL_TYPE(CommandLineArguments, command_line_arguments, FG, COMMAND_LINE_ARGUMENTS, GtkBox)

G_END_DECLS

auto create() -> CommandLineArguments*;

void add_list(const std::vector<std::string>& list);

auto get_list() -> std::vector<std::string>;

}  // namespace ui::command_line_arguments
