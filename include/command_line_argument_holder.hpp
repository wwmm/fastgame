#pragma once

#include <gtk/gtk.h>
#include "util.hpp"

namespace ui::holders {

G_BEGIN_DECLS

#define FG_TYPE_COMMAND_LINE_ARGUMENT_HOLDER (command_line_argument_holder_get_type())

G_DECLARE_FINAL_TYPE(CommandLineArgumentHolder, command_line_argument_holder, FG, COMMAND_LINE_ARGUMENT_HOLDER, GObject)

G_END_DECLS

struct CommandLineArgumentHolderData {
 public:
  std::string value;
};

struct _CommandLineArgumentHolder {
  GObject parent_instance;

  CommandLineArgumentHolderData* data;
};

auto create_cmd_holder(const std::string& value) -> CommandLineArgumentHolder*;

}  // namespace ui::holders