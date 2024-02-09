#include "environment_variable_holder.hpp"
#include "util.hpp"

namespace ui::holders {

using namespace std::string_literals;

auto constexpr log_tag = "environment variable holder: ";

enum { PROP_0, PROP_NAME, PROP_VALUE };

G_DEFINE_TYPE(EnvironmentVariableHolder, environment_variable_holder, G_TYPE_OBJECT);

void environment_variable_set_property(GObject* object, guint prop_id, const GValue* value, GParamSpec* pspec) {
  auto* self = FG_ENVIRONMENT_VARIABLE_HOLDER(object);

  switch (prop_id) {
    case PROP_NAME:
      self->data->name = g_value_get_string(value);
      break;
    case PROP_VALUE:
      self->data->value = g_value_get_string(value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

void environment_variable_get_property(GObject* object, guint prop_id, GValue* value, GParamSpec* pspec) {
  auto* self = FG_ENVIRONMENT_VARIABLE_HOLDER(object);

  switch (prop_id) {
    case PROP_NAME:
      g_value_set_string(value, self->data->name.c_str());
      break;
    case PROP_VALUE:
      g_value_set_string(value, self->data->value.c_str());
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

void environment_variable_holder_finalize(GObject* object) {
  auto* self = FG_ENVIRONMENT_VARIABLE_HOLDER(object);

  util::debug(log_tag + self->data->name + " holder object finalized"s);

  delete self->data;

  G_OBJECT_CLASS(environment_variable_holder_parent_class)->finalize(object);
}

void environment_variable_holder_class_init(EnvironmentVariableHolderClass* klass) {
  auto* object_class = G_OBJECT_CLASS(klass);

  object_class->finalize = environment_variable_holder_finalize;
  object_class->set_property = environment_variable_set_property;
  object_class->get_property = environment_variable_get_property;

  g_object_class_install_property(object_class, PROP_NAME,
                                  g_param_spec_string("name", "Environment Variable Name", "Environment Variable Name",
                                                      nullptr, G_PARAM_READWRITE));

  g_object_class_install_property(object_class, PROP_VALUE,
                                  g_param_spec_string("value", "Environment Variable Value",
                                                      "Environment Variable Value", nullptr, G_PARAM_READWRITE));
}

void environment_variable_holder_init(EnvironmentVariableHolder* self) {
  self->data = new EnvironmentVariableData();
}

auto create(const std::string& name, const std::string& value) -> EnvironmentVariableHolder* {
  auto* holder = static_cast<EnvironmentVariableHolder*>(g_object_new(FG_TYPE_ENVIRONMENT_VARIABLE_HOLDER, nullptr));

  holder->data->name = name;
  holder->data->value = value;

  return holder;
}

}  // namespace ui::holders