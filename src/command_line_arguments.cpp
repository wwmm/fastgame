#include "command_line_arguments.hpp"
#include "command_line_argument_holder.hpp"
#include "util.hpp"

namespace ui::command_line_arguments {

using namespace std::string_literals;

auto constexpr log_tag = "command_line_arguments: ";

GListStore* model;

struct _CommandLineArguments {
  GtkPopover parent_instance;

  GtkColumnView* columnview;
};

G_DEFINE_TYPE(CommandLineArguments, command_line_arguments, GTK_TYPE_BOX)

void on_add_line([[maybe_unused]] CommandLineArguments* self, [[maybe_unused]] GtkButton* btn) {
  auto* holder = ui::holders::create_cmd_holder("");

  g_list_store_append(model, holder);

  g_object_unref(holder);
}

void on_value_changed(GtkEditableLabel* label, [[maybe_unused]] GParamSpec* pspec, GtkListItem* item) {
  auto* text = gtk_editable_get_text(GTK_EDITABLE(label));

  auto child_item = gtk_list_item_get_item(item);

  auto* holder = static_cast<ui::holders::CommandLineArgumentHolder*>(child_item);

  holder->data->value = text;
}

void on_remove_line(GtkListItem* self, [[maybe_unused]] GtkButton* btn) {
  auto child_item = gtk_list_item_get_item(self);

  auto* holder = static_cast<ui::holders::CommandLineArgumentHolder*>(child_item);

  for (guint n = 0; n < g_list_model_get_n_items(G_LIST_MODEL(model)); n++) {
    auto* h = static_cast<ui::holders::CommandLineArgumentHolder*>(g_list_model_get_item(G_LIST_MODEL(model), n));

    if (h != nullptr) {
      if (h->data->value == holder->data->value) {
        g_list_store_remove(model, n);

        break;
      }

      g_object_unref(h);
    }
  }
}

void add_list(const std::vector<std::string>& list) {
  g_list_store_remove_all(model);

  for (const auto& value : list) {
    auto* holder = ui::holders::create_cmd_holder(value);

    g_list_store_append(model, holder);

    g_object_unref(holder);
  }
}

auto get_list() -> std::vector<std::string> {
  std::vector<std::string> list;

  for (guint n = 0; n < g_list_model_get_n_items(G_LIST_MODEL(model)); n++) {
    auto* holder = static_cast<ui::holders::CommandLineArgumentHolder*>(g_list_model_get_item(G_LIST_MODEL(model), n));

    if (holder != nullptr) {
      if (holder->data->value.empty()) {
        g_object_unref(holder);

        continue;
      }

      list.push_back(holder->data->value);

      g_object_unref(holder);
    }
  }

  return list;
}

void dispose(GObject* object) {
  util::debug(log_tag + "disposed"s);

  G_OBJECT_CLASS(command_line_arguments_parent_class)->dispose(object);
}

void finalize(GObject* object) {
  util::debug(log_tag + "finalized"s);

  G_OBJECT_CLASS(command_line_arguments_parent_class)->finalize(object);
}

void command_line_arguments_class_init(CommandLineArgumentsClass* klass) {
  auto* object_class = G_OBJECT_CLASS(klass);
  auto* widget_class = GTK_WIDGET_CLASS(klass);

  object_class->dispose = dispose;
  object_class->finalize = finalize;

  gtk_widget_class_set_template_from_resource(widget_class, "/com/github/wwmm/fastgame/ui/command_line_arguments.ui");

  gtk_widget_class_bind_template_child(widget_class, CommandLineArguments, columnview);

  gtk_widget_class_bind_template_callback(widget_class, on_add_line);
  gtk_widget_class_bind_template_callback(widget_class, on_remove_line);
  gtk_widget_class_bind_template_callback(widget_class, on_value_changed);
}

void command_line_arguments_init(CommandLineArguments* self) {
  gtk_widget_init_template(GTK_WIDGET(self));

  model = g_list_store_new(ui::holders::command_line_argument_holder_get_type());

  auto* selection = gtk_no_selection_new(G_LIST_MODEL(model));

  gtk_column_view_set_model(self->columnview, GTK_SELECTION_MODEL(selection));

  g_object_unref(selection);
}

auto create() -> CommandLineArguments* {
  return static_cast<CommandLineArguments*>(g_object_new(FG_TYPE_COMMAND_LINE_ARGUMENTS, nullptr));
}

}  // namespace ui::command_line_arguments
