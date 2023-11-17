#include "environment_variables.hpp"

namespace ui::environment_variables {

using namespace std::string_literals;

auto constexpr log_tag = "environment_variables: ";

GListStore* model;

struct _EnvironmentVariables {
  GtkPopover parent_instance;

  GtkColumnView* columnview;
};

G_DEFINE_TYPE(EnvironmentVariables, environment_variables, GTK_TYPE_BOX)

void on_add_line([[maybe_unused]] EnvironmentVariables* self, [[maybe_unused]] GtkButton* btn) {
  auto* holder = ui::holders::create("", "");

  g_list_store_append(model, holder);

  g_object_unref(holder);
}

void on_name_changed(GtkEditableLabel* label, [[maybe_unused]] GParamSpec* pspec, GtkListItem* item) {
  auto* text = gtk_editable_get_text(GTK_EDITABLE(label));

  auto child_item = gtk_list_item_get_item(item);

  auto* holder = static_cast<ui::holders::EnvironmentVariableHolder*>(child_item);

  holder->data->name = text;
}

void on_value_changed(GtkEditableLabel* label, [[maybe_unused]] GParamSpec* pspec, GtkListItem* item) {
  auto* text = gtk_editable_get_text(GTK_EDITABLE(label));

  auto child_item = gtk_list_item_get_item(item);

  auto* holder = static_cast<ui::holders::EnvironmentVariableHolder*>(child_item);

  holder->data->value = text;
}

void on_remove_line(GtkListItem* self, [[maybe_unused]] GtkButton* btn) {
  auto child_item = gtk_list_item_get_item(self);

  auto* holder = static_cast<ui::holders::EnvironmentVariableHolder*>(child_item);

  for (guint n = 0; n < g_list_model_get_n_items(G_LIST_MODEL(model)); n++) {
    auto* h = static_cast<ui::holders::EnvironmentVariableHolder*>(g_list_model_get_item(G_LIST_MODEL(model), n));

    if (h != nullptr) {
      if (h->data->name == holder->data->name) {
        g_list_store_remove(model, n);

        break;
      }

      g_object_unref(h);
    }
  }
}

void add_list(const std::vector<std::string>& list) {
  g_list_store_remove_all(model);

  for (const auto& key_and_value : list) {
    int delimiter_position = key_and_value.find('=');

    auto key = key_and_value.substr(0, delimiter_position);
    auto value = key_and_value.substr(delimiter_position + 1);

    auto* holder = ui::holders::create(key, value);

    g_list_store_append(model, holder);

    g_object_unref(holder);
  }
}

auto get_list() -> std::vector<std::string> {
  std::vector<std::string> list;

  for (guint n = 0; n < g_list_model_get_n_items(G_LIST_MODEL(model)); n++) {
    auto* holder = static_cast<ui::holders::EnvironmentVariableHolder*>(g_list_model_get_item(G_LIST_MODEL(model), n));

    if (holder != nullptr) {
      if (holder->data->name.empty()) {
        g_object_unref(holder);

        continue;
      }

      list.push_back(holder->data->name + "=" + holder->data->value);

      g_object_unref(holder);
    }
  }

  return list;
}

void dispose(GObject* object) {
  util::debug(log_tag + "disposed"s);

  G_OBJECT_CLASS(environment_variables_parent_class)->dispose(object);
}

void finalize(GObject* object) {
  util::debug(log_tag + "finalized"s);

  G_OBJECT_CLASS(environment_variables_parent_class)->finalize(object);
}

void environment_variables_class_init(EnvironmentVariablesClass* klass) {
  auto* object_class = G_OBJECT_CLASS(klass);
  auto* widget_class = GTK_WIDGET_CLASS(klass);

  object_class->dispose = dispose;
  object_class->finalize = finalize;

  gtk_widget_class_set_template_from_resource(widget_class, "/com/github/wwmm/fastgame/ui/environment_variables.ui");

  gtk_widget_class_bind_template_child(widget_class, EnvironmentVariables, columnview);

  gtk_widget_class_bind_template_callback(widget_class, on_add_line);
  gtk_widget_class_bind_template_callback(widget_class, on_remove_line);
  gtk_widget_class_bind_template_callback(widget_class, on_name_changed);
  gtk_widget_class_bind_template_callback(widget_class, on_value_changed);
}

void environment_variables_init(EnvironmentVariables* self) {
  gtk_widget_init_template(GTK_WIDGET(self));

  model = g_list_store_new(ui::holders::environment_variable_holder_get_type());

  auto* selection = gtk_no_selection_new(G_LIST_MODEL(model));

  gtk_column_view_set_model(self->columnview, GTK_SELECTION_MODEL(selection));

  g_object_unref(selection);
}

auto create() -> EnvironmentVariables* {
  return static_cast<EnvironmentVariables*>(g_object_new(FG_TYPE_ENVIRONMENT_VARIABLES, nullptr));
}

}  // namespace ui::environment_variables
