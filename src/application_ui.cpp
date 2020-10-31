#include "application_ui.hpp"
#include <glibmm/i18n.h>
#include <gtkmm/icontheme.h>
#include <gtkmm/settings.h>
#include "amdgpu.hpp"
#include "cpu.hpp"
#include "environment_variables.hpp"
#include "memory.hpp"
#include "util.hpp"

ApplicationUi::ApplicationUi(BaseObjectType* cobject,
                             const Glib::RefPtr<Gtk::Builder>& builder,
                             Application* application)
    : Gtk::ApplicationWindow(cobject), app(application), settings(app->settings) {
  Gtk::IconTheme::get_default()->add_resource_path("/com/github/wwmm/fastgame/icons");

  // loading glade widgets

  builder->get_widget("stack", stack);

  EnvironmentVariables::add_to_stack(stack, app);
  Cpu::add_to_stack(stack, app);
  Amdgpu::add_to_stack(stack, app);
  Memory::add_to_stack(stack, app);

  stack->connect_property_changed("visible-child",
                                  sigc::mem_fun(*this, &ApplicationUi::on_stack_visible_child_changed));

  // binding glade widgets to gsettings keys

  auto flag = Gio::SettingsBindFlags::SETTINGS_BIND_DEFAULT;

  settings->bind("use-dark-theme", Gtk::Settings::get_default().get(), "gtk_application_prefer_dark_theme", flag);

  // restore window size

  auto window_width = settings->get_int("window-width");
  auto window_height = settings->get_int("window-height");

  if (window_width > 0 && window_height > 0) {
    set_default_size(window_width, window_height);
  }
}

ApplicationUi::~ApplicationUi() {
  for (auto& c : connections) {
    c.disconnect();
  }

  util::debug(log_tag + "destroyed");
}

auto ApplicationUi::create(Application* app_this) -> ApplicationUi* {
  auto builder = Gtk::Builder::create_from_resource("/com/github/wwmm/fastgame/ui/application.glade");

  ApplicationUi* window = nullptr;

  builder->get_widget_derived("ApplicationUi", window, app_this);

  return window;
}

void ApplicationUi::on_stack_visible_child_changed() {
  auto name = stack->get_visible_child_name();

  // if (name == std::string("sink_inputs")) {
  //   update_headerbar_subtitle(0);

  //   presets_menu_label->set_text(settings->get_string("last-used-output-preset"));
  // } else if (name == std::string("source_outputs")) {
  //   update_headerbar_subtitle(1);

  //   presets_menu_label->set_text(settings->get_string("last-used-input-preset"));
  // } else if (name == std::string("pulse_info")) {
  //   update_headerbar_subtitle(2);
  // }
}
