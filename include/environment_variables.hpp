#ifndef ENVIRONMENT_VARIABLES_HPP
#define ENVIRONMENT_VARIABLES_HPP

#include <giomm/settings.h>
#include <glibmm/i18n.h>
#include <gtkmm/builder.h>
#include <gtkmm/button.h>
#include <gtkmm/cellrenderertext.h>
#include <gtkmm/grid.h>
#include <gtkmm/liststore.h>
#include <gtkmm/stack.h>
#include <gtkmm/treeview.h>
#include "application.hpp"

class EnvironmentVariables : public Gtk::Grid {
 public:
  EnvironmentVariables(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder, Application* application);
  EnvironmentVariables(const EnvironmentVariables&) = delete;
  auto operator=(const EnvironmentVariables&) -> EnvironmentVariables& = delete;
  EnvironmentVariables(const EnvironmentVariables&&) = delete;
  auto operator=(const EnvironmentVariables &&) -> EnvironmentVariables& = delete;
  ~EnvironmentVariables() override;

  static void add_to_stack(Gtk::Stack* stack, Application* app);

 private:
  std::string log_tag = "environment variables: ";

  Glib::RefPtr<Gio::Settings> settings;

  Application* app = nullptr;

  Gtk::Button *button_add = nullptr, *button_remove = nullptr;

  Gtk::TreeView* treeview = nullptr;

  Gtk::ListStore* liststore = nullptr;

  Gtk::CellRendererText *cell_name = nullptr, *cell_value = nullptr;

  std::vector<sigc::connection> connections;
};

#endif
