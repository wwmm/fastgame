#include "application.hpp"

namespace app {

using namespace std::string_literals;

auto constexpr log_tag = "application: ";

G_DEFINE_TYPE(Application, application, ADW_TYPE_APPLICATION)

void hide_all_windows(GApplication* app) {
  auto* list = gtk_application_get_windows(GTK_APPLICATION(app));

  while (list != nullptr) {
    auto* window = list->data;
    auto* next = list->next;

    gtk_window_destroy(GTK_WINDOW(window));

    list = next;
  }
}

void application_class_init(ApplicationClass* klass) {
  auto* application_class = G_APPLICATION_CLASS(klass);

  application_class->startup = [](GApplication* gapp) {
    G_APPLICATION_CLASS(application_parent_class)->startup(gapp);

    auto* self = EE_APP(gapp);

    self->data = new Data();

    self->settings = g_settings_new("com.github.wwmm.fastgame");
  };

  application_class->activate = [](GApplication* gapp) {
    if (gtk_application_get_active_window(GTK_APPLICATION(gapp)) == nullptr) {
      G_APPLICATION_CLASS(application_parent_class)->activate(gapp);

      // auto* window = ui::application_window::create(gapp);

      // gtk_window_present(GTK_WINDOW(window));
    }
  };

  application_class->shutdown = [](GApplication* gapp) {
    G_APPLICATION_CLASS(application_parent_class)->shutdown(gapp);

    auto* self = EE_APP(gapp);

    for (auto& c : self->data->connections) {
      c.disconnect();
    }

    for (auto& handler_id : self->data->gconnections) {
      g_signal_handler_disconnect(self->settings, handler_id);
    }

    self->data->connections.clear();

    g_object_unref(self->settings);

    delete self->data;

    self->data = nullptr;

    util::debug(log_tag + "shutting down..."s);
  };
}

void application_init(Application* self) {
  std::array<GActionEntry, 5> entries{};

  entries[0] = {
      "quit", [](GSimpleAction* action, GVariant* parameter, gpointer app) { g_application_quit(G_APPLICATION(app)); },
      nullptr, nullptr, nullptr};

  entries[1] = {"about",
                [](GSimpleAction* action, GVariant* parameter, gpointer gapp) {
                  std::array<const char*, 2> authors = {"Wellington Wallace", nullptr};

                  gtk_show_about_dialog(gtk_application_get_active_window(GTK_APPLICATION(gapp)), "program-name",
                                        "FastGame", "version", VERSION, "comments",
                                        _("Optimize system performance for games"), "authors", authors.data(),
                                        "logo-icon-name", "fastgame", "license-type", GTK_LICENSE_GPL_3_0, "website",
                                        "https://github.com/wwmm/fastgame", nullptr);
                },
                nullptr, nullptr, nullptr};

  entries[2] = {"fullscreen",
                [](GSimpleAction* action, GVariant* parameter, gpointer gapp) {
                  auto* self = EE_APP(gapp);

                  auto state = g_settings_get_boolean(self->settings, "window-fullscreen") != 0;

                  if (state) {
                    gtk_window_unfullscreen(GTK_WINDOW(gtk_application_get_active_window(GTK_APPLICATION(gapp))));

                    g_settings_set_boolean(self->settings, "window-fullscreen", 0);
                  } else {
                    gtk_window_fullscreen(GTK_WINDOW(gtk_application_get_active_window(GTK_APPLICATION(gapp))));

                    g_settings_set_boolean(self->settings, "window-fullscreen", 1);
                  }
                },
                nullptr, nullptr, nullptr};

  entries[3] = {"shortcuts",
                [](GSimpleAction* action, GVariant* parameter, gpointer gapp) {
                  auto* builder = gtk_builder_new_from_resource("/com/github/wwmm/easyeffects/ui/shortcuts.ui");

                  auto* window = GTK_SHORTCUTS_WINDOW(gtk_builder_get_object(builder, "window"));

                  gtk_window_set_transient_for(GTK_WINDOW(window),
                                               GTK_WINDOW(gtk_application_get_active_window(GTK_APPLICATION(gapp))));

                  gtk_window_present(GTK_WINDOW(window));

                  g_object_unref(builder);
                },
                nullptr, nullptr, nullptr};

  entries[4] = {"hide_windows",
                [](GSimpleAction* action, GVariant* parameter, gpointer app) { hide_all_windows(G_APPLICATION(app)); },
                nullptr, nullptr, nullptr};

  g_action_map_add_action_entries(G_ACTION_MAP(self), entries.data(), entries.size(), self);

  std::array<const char*, 2> quit_accels = {"<Ctrl>Q", nullptr};
  std::array<const char*, 2> hide_windows_accels = {"<Ctrl>W", nullptr};

  std::array<const char*, 2> fullscreen_accels = {"F11", nullptr};

  gtk_application_set_accels_for_action(GTK_APPLICATION(self), "app.quit", quit_accels.data());
  gtk_application_set_accels_for_action(GTK_APPLICATION(self), "app.hide_windows", hide_windows_accels.data());
  gtk_application_set_accels_for_action(GTK_APPLICATION(self), "app.fullscreen", fullscreen_accels.data());
}

auto application_new() -> GApplication* {
  g_set_application_name("FastGame");

  auto* app = g_object_new(EE_TYPE_APPLICATION, "application-id", "com.github.wwmm.fastgame", "flags",
                           G_APPLICATION_FLAGS_NONE, nullptr);

  return G_APPLICATION(app);
}

}  // namespace app

// Application::Application() : Gtk::Application("com.github.wwmm.fastgame", Gio::APPLICATION_FLAGS_NONE) {
//   Glib::set_application_name("FastGame");
// }

// auto Application::create() -> Glib::RefPtr<Application> {
//   return Glib::RefPtr<Application>(new Application());
// }

// void Application::on_startup() {
//   Gtk::Application::on_startup();

//   util::debug(log_tag + "fastgame version: " + std::string(VERSION));

//   settings = Gio::Settings::create("com.github.wwmm.fastgame");

//   create_actions();
// }

// void Application::on_activate() {
//   if (get_active_window() == nullptr) {
//     /*
//       Note to myself: do not wrap this pointer in a smart pointer. Causes memory leaks when closing the window
//       because GTK reference counting system will see that there is still someone with an object reference and it
//       won't free the widgets.
//     */
//     auto* window = ApplicationUi::create(this);

//     add_window(*window);

//     window->signal_hide().connect([&, window]() {
//       int width = 0;
//       int height = 0;

//       window->get_size(width, height);

//       settings->set_int("window-width", width);
//       settings->set_int("window-height", height);

//       delete window;
//     });

//     window->show_all();
//   }
// }

// void Application::create_actions() {
//   add_action("about", [&]() {
//     auto builder = Gtk::Builder::create_from_resource("/com/github/wwmm/fastgame/about.glade");

//     auto* dialog = (Gtk::Dialog*)builder->get_object("about_dialog").get();

//     dialog->signal_response().connect([=](auto response_id) {
//       switch (response_id) {
//         case Gtk::RESPONSE_CLOSE:
//         case Gtk::RESPONSE_CANCEL:
//         case Gtk::RESPONSE_DELETE_EVENT: {
//           dialog->hide();
//           util::debug(log_tag + "hiding the about dialog window");
//           break;
//         }
//         default:
//           util::debug(log_tag + "unexpected about dialog response!");
//           break;
//       }
//     });

//     dialog->set_transient_for(*get_active_window());

//     dialog->show();

//     // Bring it to the front, in case it was already shown:
//     dialog->present();
//   });

//   add_action("quit", [&] {
//     auto* window = get_active_window();

//     window->hide();
//   });

//   set_accel_for_action("app.quit", "<Ctrl>Q");
// }
