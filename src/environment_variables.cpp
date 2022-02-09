#include "environment_variables.hpp"
#include "environment_variable_holder.hpp"

namespace ui::environmental_variables {

using namespace std::string_literals;

auto constexpr log_tag = "environmental_variables: ";

struct Data {
 public:
  ~Data() { util::debug(log_tag + "data struct destroyed"s); }

  app::Application* application;

  std::vector<sigc::connection> connections;

  std::vector<gulong> gconnections;
};

struct _EnvironmentVariables {
  GtkPopover parent_instance;

  GtkColumnView* columnview;

  GListStore* model;

  GSettings* settings;

  Data* data;
};

G_DEFINE_TYPE(EnvironmentVariables, environment_variables, GTK_TYPE_BOX)

void setup(EnvironmentVariables* self, app::Application* application) {
  self->data->application = application;
}

void dispose(GObject* object) {
  auto* self = FG_ENVIRONMENT_VARIABLES(object);

  for (auto& c : self->data->connections) {
    c.disconnect();
  }

  for (auto& handler_id : self->data->gconnections) {
    g_signal_handler_disconnect(self->settings, handler_id);
  }

  self->data->connections.clear();
  self->data->gconnections.clear();

  g_object_unref(self->settings);

  util::debug(log_tag + "disposed"s);

  G_OBJECT_CLASS(environment_variables_parent_class)->dispose(object);
}

void finalize(GObject* object) {
  auto* self = FG_ENVIRONMENT_VARIABLES(object);

  delete self->data;

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

  // gtk_widget_class_bind_template_callback(widget_class, create_preset);
  // gtk_widget_class_bind_template_callback(widget_class, import_preset);
}

void environment_variables_init(EnvironmentVariables* self) {
  gtk_widget_init_template(GTK_WIDGET(self));

  self->data = new Data();

  self->settings = g_settings_new("com.github.wwmm.fastgame");

  self->model = g_list_store_new(ui::holders::environment_variable_holder_get_type());

  auto* selection = gtk_single_selection_new(G_LIST_MODEL(self->model));

  gtk_column_view_set_model(self->columnview, GTK_SELECTION_MODEL(selection));

  g_object_unref(selection);

  auto* holder = ui::holders::create("v", "1");

  g_list_store_append(self->model, holder);
}

auto create() -> EnvironmentVariables* {
  return static_cast<EnvironmentVariables*>(g_object_new(FG_TYPE_ENVIRONMENT_VARIABLES, nullptr));
}

}  // namespace ui::environmental_variables

// EnvironmentVariables::EnvironmentVariables(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder)
//     : Gtk::Grid(cobject), settings(Gio::Settings::create("com.github.wwmm.fastgame")) {
//   // loading glade widgets

//   builder->get_widget("treeview", treeview);
//   builder->get_widget("button_add", button_add);
//   builder->get_widget("button_remove", button_remove);

//   liststore = dynamic_cast<Gtk::ListStore*>(builder->get_object("liststore").get());
//   cell_name = dynamic_cast<Gtk::CellRendererText*>(builder->get_object("cell_name").get());
//   cell_value = dynamic_cast<Gtk::CellRendererText*>(builder->get_object("cell_value").get());

//   // signals connection

//   button_add->signal_clicked().connect([=]() { liststore->append(); });

//   button_remove->signal_clicked().connect([=]() {
//     auto iter = treeview->get_selection()->get_selected();

//     if (iter) {
//       liststore->erase(iter);
//     }
//   });

//   cell_name->signal_edited().connect([=](const Glib::ustring& path, const Glib::ustring& new_text) {
//     auto iter = liststore->get_iter(path);

//     iter->set_value(0, new_text);
//   });

//   cell_value->signal_edited().connect([=](const Glib::ustring& path, const Glib::ustring& new_text) {
//     auto iter = liststore->get_iter(path);

//     iter->set_value(1, new_text);
//   });
// }

// EnvironmentVariables::~EnvironmentVariables() {
//   util::debug(log_tag + "destroyed");
// }

// auto EnvironmentVariables::add_to_stack(Gtk::Stack* stack) -> EnvironmentVariables* {
//   auto builder = Gtk::Builder::create_from_resource("/com/github/wwmm/fastgame/ui/environment_variables.glade");

//   EnvironmentVariables* ui = nullptr;

//   builder->get_widget_derived("widgets_grid", ui);

//   stack->add(*ui, "environment_variables", _("Environment Variables"));

//   return ui;
// }

// auto EnvironmentVariables::get_variables() -> std::vector<std::string> {
//   std::vector<std::string> list;

//   auto children = liststore->children();

//   for (const auto& row : children) {
//     std::string variable;
//     std::string value;

//     row->get_value(0, variable);
//     row->get_value(1, value);

//     if (variable.empty()) {
//       continue;
//     }

//     variable.append("=");
//     variable.append(value);

//     list.emplace_back(variable);
//   }

//   return list;
// }

// void EnvironmentVariables::set_variables(const std::vector<std::string>& list) {
//   liststore->clear();

//   for (const auto& key_and_value : list) {
//     int delimiter_position = key_and_value.find('=');

//     auto key = key_and_value.substr(0, delimiter_position);
//     auto value = key_and_value.substr(delimiter_position + 1);

//     auto row = *liststore->append();

//     row.set_value(0, key);
//     row.set_value(1, value);
//   }
// }