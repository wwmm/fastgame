#ifndef ENVIRONMENT_VARIABLES_HPP
#define ENVIRONMENT_VARIABLES_HPP

#include <giomm/settings.h>
#include <gtkmm/builder.h>
#include <gtkmm/button.h>
#include <gtkmm/cellrenderertext.h>
#include <gtkmm/grid.h>
#include <gtkmm/liststore.h>
#include <gtkmm/stack.h>
#include <gtkmm/treeview.h>

class EnvironmentVariables : public Gtk::Grid {
 public:
  EnvironmentVariables(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder);
  EnvironmentVariables(const EnvironmentVariables&) = delete;
  auto operator=(const EnvironmentVariables&) -> EnvironmentVariables& = delete;
  EnvironmentVariables(const EnvironmentVariables&&) = delete;
  auto operator=(const EnvironmentVariables &&) -> EnvironmentVariables& = delete;
  ~EnvironmentVariables() override;

  static auto add_to_stack(Gtk::Stack* stack) -> EnvironmentVariables*;

  auto get_variables() -> std::vector<std::string>;

  void set_variables(const std::vector<std::string>& list);

 private:
  std::string log_tag = "environment variables: ";

  Glib::RefPtr<Gio::Settings> settings;

  Gtk::Button *button_add = nullptr, *button_remove = nullptr;

  Gtk::TreeView* treeview = nullptr;

  Gtk::ListStore* liststore = nullptr;

  Gtk::CellRendererText *cell_name = nullptr, *cell_value = nullptr;
};

#endif
