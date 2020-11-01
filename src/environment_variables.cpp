#include "environment_variables.hpp"
#include <glibmm/i18n.h>
#include <boost/algorithm/string/join.hpp>
#include "gtkmm/treemodelcolumn.h"
#include "util.hpp"

EnvironmentVariables::EnvironmentVariables(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder)
    : Gtk::Grid(cobject), settings(Gio::Settings::create("com.github.wwmm.fastgame")) {
  // loading glade widgets

  builder->get_widget("treeview", treeview);
  builder->get_widget("button_add", button_add);
  builder->get_widget("button_remove", button_remove);

  liststore = dynamic_cast<Gtk::ListStore*>(builder->get_object("liststore").get());
  cell_name = dynamic_cast<Gtk::CellRendererText*>(builder->get_object("cell_name").get());
  cell_value = dynamic_cast<Gtk::CellRendererText*>(builder->get_object("cell_value").get());

  // signals connection

  button_add->signal_clicked().connect([=]() { liststore->append(); });

  cell_name->signal_edited().connect([=](const Glib::ustring& path, const Glib::ustring& new_text) {
    auto iter = liststore->get_iter(path);

    iter->set_value(0, new_text);
  });

  cell_value->signal_edited().connect([=](const Glib::ustring& path, const Glib::ustring& new_text) {
    auto iter = liststore->get_iter(path);

    iter->set_value(1, new_text);
  });
}

EnvironmentVariables::~EnvironmentVariables() {
  util::debug(log_tag + "destroyed");
}

auto EnvironmentVariables::add_to_stack(Gtk::Stack* stack) -> EnvironmentVariables* {
  auto builder = Gtk::Builder::create_from_resource("/com/github/wwmm/fastgame/ui/environment_variables.glade");

  EnvironmentVariables* ui = nullptr;

  builder->get_widget_derived("widgets_grid", ui);

  stack->add(*ui, "environment_variables", _("Environment Variables"));

  return ui;
}

auto EnvironmentVariables::get_variables() -> std::string {
  std::vector<std::string> variables;

  auto children = liststore->children();

  for (const auto& row : children) {
    std::string variable;
    std::string value;

    row->get_value(0, variable);
    row->get_value(1, value);

    variable.append("=");
    variable.append(value);

    variables.emplace_back(variable);
  }

  return boost::algorithm::join(variables, ";");
}