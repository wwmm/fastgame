#pragma once

#include <gtk/gtk.h>
#include "util.hpp"

namespace ui::holders {

G_BEGIN_DECLS

#define FG_TYPE_ENVIRONMENT_VARIABLE_HOLDER (environment_variable_holder_get_type())

G_DECLARE_FINAL_TYPE(EnvironmentVariableHolder, environment_variable_holder, FG, ENVIRONMENT_VARIABLE_HOLDER, GObject)

G_END_DECLS

struct EnvironmentVariableData {
 public:
  std::string name, value;
};

struct _EnvironmentVariableHolder {
  GObject parent_instance;

  EnvironmentVariableData* data;
};

auto create(const std::string& name, const std::string& value) -> EnvironmentVariableHolder*;

}  // namespace ui::holders