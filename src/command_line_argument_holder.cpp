#include "command_line_argument_holder.hpp"

namespace ui::holders {

using namespace std::string_literals;

auto constexpr log_tag = "command_line_argument_holder: ";

enum { PROP_0, PROP_VALUE };

G_DEFINE_TYPE(CommandLineArgumentHolder, command_line_argument_holder, G_TYPE_OBJECT);

void command_line_argument_set_property(GObject* object, guint prop_id, const GValue* value, GParamSpec* pspec) {
  auto* self = FG_COMMAND_LINE_ARGUMENT_HOLDER(object);

  switch (prop_id) {
    case PROP_VALUE:
      self->data->value = g_value_get_string(value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

void command_line_argument_get_property(GObject* object, guint prop_id, GValue* value, GParamSpec* pspec) {
  auto* self = FG_COMMAND_LINE_ARGUMENT_HOLDER(object);

  switch (prop_id) {
    case PROP_VALUE:
      g_value_set_string(value, self->data->value.c_str());
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

void command_line_argument_holder_finalize(GObject* object) {
  auto* self = FG_COMMAND_LINE_ARGUMENT_HOLDER(object);

  util::debug(log_tag + self->data->value + "holder object finalized"s);

  delete self->data;

  G_OBJECT_CLASS(command_line_argument_holder_parent_class)->finalize(object);
}

void command_line_argument_holder_class_init(CommandLineArgumentHolderClass* klass) {
  auto* object_class = G_OBJECT_CLASS(klass);

  object_class->finalize = command_line_argument_holder_finalize;
  object_class->set_property = command_line_argument_set_property;
  object_class->get_property = command_line_argument_get_property;

  g_object_class_install_property(object_class, PROP_VALUE,
                                  g_param_spec_string("value", "Command Line Argument Value",
                                                      "Command Line Argument Value", nullptr, G_PARAM_READWRITE));
}

void command_line_argument_holder_init(CommandLineArgumentHolder* self) {
  self->data = new CommandLineArgumentHolderData();
}

auto create_cmd_holder(const std::string& value) -> CommandLineArgumentHolder* {
  auto* holder = static_cast<CommandLineArgumentHolder*>(g_object_new(FG_TYPE_COMMAND_LINE_ARGUMENT_HOLDER, nullptr));

  holder->data->value = value;

  return holder;
}

}  // namespace ui::holders