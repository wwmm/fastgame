#pragma once

#include <adwaita.h>
#include <filesystem>
#include "application.hpp"
#include "environment_variable_holder.hpp"

namespace ui::environmental_variables {

G_BEGIN_DECLS

#define FG_TYPE_ENVIRONMENT_VARIABLES (environment_variables_get_type())

G_DECLARE_FINAL_TYPE(EnvironmentVariables, environment_variables, FG, ENVIRONMENT_VARIABLES, GtkBox)

G_END_DECLS

auto create() -> EnvironmentVariables*;

void setup(EnvironmentVariables* self, app::Application* application);

}  // namespace ui::environmental_variables

// class EnvironmentVariables : public Gtk::Grid {
//  public:
//   EnvironmentVariables(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder);
//   EnvironmentVariables(const EnvironmentVariables&) = delete;
//   auto operator=(const EnvironmentVariables&) -> EnvironmentVariables& = delete;
//   EnvironmentVariables(const EnvironmentVariables&&) = delete;
//   auto operator=(const EnvironmentVariables&&) -> EnvironmentVariables& = delete;
//   ~EnvironmentVariables() override;

//   static auto add_to_stack(Gtk::Stack* stack) -> EnvironmentVariables*;

//   auto get_variables() -> std::vector<std::string>;

//   void set_variables(const std::vector<std::string>& list);

//  private:
//   std::string log_tag = "environment variables: ";

//   Glib::RefPtr<Gio::Settings> settings;

//   Gtk::Button *button_add = nullptr, *button_remove = nullptr;

//   Gtk::TreeView* treeview = nullptr;

//   Gtk::ListStore* liststore = nullptr;

//   Gtk::CellRendererText *cell_name = nullptr, *cell_value = nullptr;
// };
