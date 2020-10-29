#ifndef APPLICATION_WINDOW_HPP
#define APPLICATION_WINDOW_HPP

#include <gtkmm/adjustment.h>
#include <gtkmm/applicationwindow.h>
#include <gtkmm/builder.h>
#include <gtkmm/grid.h>
#include <gtkmm/stack.h>
#include "application.hpp"

class ApplicationUi : public Gtk::ApplicationWindow {
 public:
  ApplicationUi(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder, Application* application);
  ApplicationUi(const ApplicationUi&) = delete;
  auto operator=(const ApplicationUi&) -> ApplicationUi& = delete;
  ApplicationUi(const ApplicationUi&&) = delete;
  auto operator=(const ApplicationUi &&) -> ApplicationUi& = delete;
  ~ApplicationUi() override;

  static auto create(Application* app) -> ApplicationUi*;

 private:
  std::string log_tag = "application_ui: ";

  Application* app;

  Glib::RefPtr<Gio::Settings> settings;

  std::locale syslocale = std::locale("");

  Gtk::Stack* stack = nullptr;

  std::vector<sigc::connection> connections;

  static void get_object(const Glib::RefPtr<Gtk::Builder>& builder,
                         const std::string& name,
                         Glib::RefPtr<Gtk::Adjustment>& object) {
    object = Glib::RefPtr<Gtk::Adjustment>::cast_dynamic(builder->get_object(name));
  }

  void on_stack_visible_child_changed();
};

#endif
