#include "application.hpp"
#include <glibmm.h>
#include <glibmm/i18n.h>
#include <gtkmm/dialog.h>
#include "application_ui.hpp"
#include "config.h"
#include "util.hpp"

Application::Application() : Gtk::Application("com.github.wwmm.fastgame", Gio::APPLICATION_FLAGS_NONE) {
  Glib::set_application_name("FastGame");
}

Application::~Application() {
  util::debug(log_tag + " destroyed");
}

auto Application::create() -> Glib::RefPtr<Application> {
  return Glib::RefPtr<Application>(new Application());
}

void Application::on_startup() {
  Gtk::Application::on_startup();

  util::debug(log_tag + "fastgame version: " + std::string(VERSION));

  settings = Gio::Settings::create("com.github.wwmm.fastgame");

  create_actions();
}

void Application::on_activate() {
  if (get_active_window() == nullptr) {
    /*
      Note to myself: do not wrap this pointer in a smart pointer. Causes memory leaks when closing the window because
      GTK reference counting system will see that there is still someone with an object reference and it won't free the
      widgets.
    */
    auto* window = ApplicationUi::create(this);

    add_window(*window);

    window->signal_hide().connect([&, window]() {
      int width = 0;
      int height = 0;

      window->get_size(width, height);

      settings->set_int("window-width", width);
      settings->set_int("window-height", height);

      delete window;
    });

    window->show_all();
  }
}

void Application::create_actions() {
  add_action("about", [&]() {
    auto builder = Gtk::Builder::create_from_resource("/com/github/wwmm/fastgame/about.glade");

    auto* dialog = (Gtk::Dialog*)builder->get_object("about_dialog").get();

    dialog->signal_response().connect([=](auto response_id) {
      switch (response_id) {
        case Gtk::RESPONSE_CLOSE:
        case Gtk::RESPONSE_CANCEL:
        case Gtk::RESPONSE_DELETE_EVENT: {
          dialog->hide();
          util::debug(log_tag + "hiding the about dialog window");
          break;
        }
        default:
          util::debug(log_tag + "unexpected about dialog response!");
          break;
      }
    });

    dialog->set_transient_for(*get_active_window());

    dialog->show();

    // Bring it to the front, in case it was already shown:
    dialog->present();
  });

  add_action("quit", [&] {
    auto* window = get_active_window();

    window->hide();
  });

  set_accel_for_action("app.quit", "<Ctrl>Q");
}
