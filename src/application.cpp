#include "application.hpp"
#include "application_ui.hpp"

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

    auto* self = FG_APP(gapp);

    self->data = new Data();

    self->settings = g_settings_new("com.github.wwmm.fastgame");
  };

  application_class->activate = [](GApplication* gapp) {
    if (gtk_application_get_active_window(GTK_APPLICATION(gapp)) == nullptr) {
      G_APPLICATION_CLASS(application_parent_class)->activate(gapp);

      auto* window = ui::application_window::create(gapp);

      gtk_window_present(GTK_WINDOW(window));
    }
  };

  application_class->shutdown = [](GApplication* gapp) {
    G_APPLICATION_CLASS(application_parent_class)->shutdown(gapp);

    auto* self = FG_APP(gapp);

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
                  auto* self = FG_APP(gapp);

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
                  auto* builder = gtk_builder_new_from_resource("/com/github/wwmm/fastgame/ui/shortcuts.ui");

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

  auto* app = g_object_new(FG_TYPE_APPLICATION, "application-id", "com.github.wwmm.fastgame", "flags",
                           G_APPLICATION_FLAGS_NONE, nullptr);

  return G_APPLICATION(app);
}

}  // namespace app
