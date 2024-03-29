#include "presets_menu.hpp"
#include "util.hpp"

namespace ui::presets_menu {

using namespace std::string_literals;

auto constexpr log_tag = "presets_menu: ";

std::filesystem::path user_presets_dir = g_get_user_config_dir() + "/fastgame"s;

sigc::signal<void(const std::string&)> save_preset;
sigc::signal<void(const std::string&)> load_preset;

struct Data {
 public:
  ~Data() { util::debug(log_tag + "data struct destroyed"s); }

  app::Application* application;
};

struct _PresetsMenu {
  GtkPopover parent_instance;

  GtkScrolledWindow* scrolled_window;

  GtkListView* listview;

  GtkText* preset_name;

  GtkStringList* string_list;

  GFileMonitor* dir_monitor = nullptr;

  Data* data;
};

G_DEFINE_TYPE(PresetsMenu, presets_menu, GTK_TYPE_POPOVER)

void create_user_directory() {
  if (!std::filesystem::is_directory(user_presets_dir)) {
    if (std::filesystem::create_directories(user_presets_dir)) {
      util::debug(log_tag + "user presets directory created: "s + user_presets_dir.string());
    } else {
      util::warning(log_tag + "failed to create user presets directory: "s + user_presets_dir.string());
    }
  } else {
    util::debug(log_tag + "user presets directory already exists: "s + user_presets_dir.string());
  }
}

auto get_presets_names() -> std::vector<std::string> {
  std::vector<std::string> names;

  for (const auto& entry : std::filesystem::directory_iterator(user_presets_dir)) {
    names.emplace_back(entry.path().stem().string());
  }

  return names;
}

void create_preset(PresetsMenu* self, [[maybe_unused]] GtkButton* button) {
  auto name = std::string(g_utf8_make_valid(gtk_editable_get_text(GTK_EDITABLE(self->preset_name)), -1));

  if (name.empty()) {
    return;
  }

  gtk_editable_set_text(GTK_EDITABLE(self->preset_name), "");

  // Truncate if longer than 100 characters

  if (name.length() > 100U) {
    name.resize(100U);
  }

  if (name.find_first_of("\\/") != std::string::npos) {
    util::debug(log_tag + " name "s + name + " has illegal file name characters. Aborting preset creation!"s);

    return;
  }

  gtk_editable_set_text(GTK_EDITABLE(self->preset_name), "");

  for (const auto& used_name : get_presets_names()) {
    if (used_name == name) {
      return;
    }
  }

  save_preset.emit(name);
}

void import_preset(PresetsMenu* self) {
  auto* active_window = gtk_application_get_active_window(GTK_APPLICATION(self->data->application));

  auto* dialog = gtk_file_dialog_new();

  gtk_file_dialog_set_title(dialog, _("Import Preset"));
  gtk_file_dialog_set_accept_label(dialog, _("Open"));

  GListStore* filters = g_list_store_new(GTK_TYPE_FILE_FILTER);

  auto* filter = gtk_file_filter_new();

  gtk_file_filter_add_pattern(filter, "*.json");
  gtk_file_filter_set_name(filter, _("Presets"));

  g_list_store_append(filters, filter);

  g_object_unref(filter);

  gtk_file_dialog_set_filters(dialog, G_LIST_MODEL(filters));

  g_object_unref(filters);

  gtk_file_dialog_open(
      dialog, active_window, nullptr,
      +[](GObject* source_object, GAsyncResult* result, [[maybe_unused]] gpointer user_data) {
        auto* dialog = GTK_FILE_DIALOG(source_object);

        auto* file = gtk_file_dialog_open_finish(dialog, result, nullptr);

        if (file != nullptr) {
          auto* path = g_file_get_path(file);

          if (std::filesystem::is_regular_file(path)) {
            auto output_path = user_presets_dir / std::filesystem::path{path};

            std::filesystem::copy_file(path, output_path, std::filesystem::copy_options::overwrite_existing);

            util::debug(log_tag + "imported preset to: "s + output_path.string());
          }

          g_free(path);

          g_object_unref(file);
        }
      },
      self);
}

void setup_listview(PresetsMenu* self, GtkListView* listview, GtkStringList* string_list) {
  auto* factory = gtk_signal_list_item_factory_new();

  // setting the factory callbacks

  g_signal_connect(
      factory, "setup",
      G_CALLBACK(+[]([[maybe_unused]] GtkSignalListItemFactory* factory, GtkListItem* item, PresetsMenu* self) {
        auto builder = gtk_builder_new_from_resource("/com/github/wwmm/fastgame/ui/preset_row.ui");

        auto* top_box = gtk_builder_get_object(builder, "top_box");
        auto* apply = gtk_builder_get_object(builder, "apply");
        auto* save = gtk_builder_get_object(builder, "save");
        auto* remove = gtk_builder_get_object(builder, "remove");

        g_object_set_data(G_OBJECT(item), "name", gtk_builder_get_object(builder, "name"));
        g_object_set_data(G_OBJECT(item), "apply", apply);
        g_object_set_data(G_OBJECT(item), "save", save);
        g_object_set_data(G_OBJECT(item), "remove", remove);

        gtk_list_item_set_activatable(item, 0);
        gtk_list_item_set_child(item, GTK_WIDGET(top_box));

        g_signal_connect(
            apply, "clicked", G_CALLBACK(+[](GtkButton* button, [[maybe_unused]] PresetsMenu* self) {
              if (auto* string_object = GTK_STRING_OBJECT(g_object_get_data(G_OBJECT(button), "string-object"));
                  string_object != nullptr) {
                auto* name = gtk_string_object_get_string(string_object);

                load_preset.emit(name);
              }
            }),
            self);

        g_signal_connect(
            save, "clicked", G_CALLBACK(+[](GtkButton* button, [[maybe_unused]] PresetsMenu* self) {
              if (auto* string_object = GTK_STRING_OBJECT(g_object_get_data(G_OBJECT(button), "string-object"));
                  string_object != nullptr) {
                auto* name = gtk_string_object_get_string(string_object);

                save_preset.emit(name);
              }
            }),
            self);

        g_signal_connect(
            remove, "clicked", G_CALLBACK(+[](GtkButton* button, [[maybe_unused]] PresetsMenu* self) {
              if (auto* string_object = GTK_STRING_OBJECT(g_object_get_data(G_OBJECT(button), "string-object"));
                  string_object != nullptr) {
                auto* name = gtk_string_object_get_string(string_object);

                auto file_path = user_presets_dir / std::filesystem::path{name + ".json"s};

                std::filesystem::remove(file_path);

                util::debug(log_tag + "removed preset file: "s + file_path.string());
              }
            }),
            self);

        g_object_unref(builder);
      }),
      self);

  g_signal_connect(factory, "bind",
                   G_CALLBACK(+[]([[maybe_unused]] GtkSignalListItemFactory* factory, GtkListItem* item,
                                  [[maybe_unused]] PresetsMenu* self) {
                     auto* label = static_cast<GtkLabel*>(g_object_get_data(G_OBJECT(item), "name"));
                     auto* apply = static_cast<GtkLabel*>(g_object_get_data(G_OBJECT(item), "apply"));
                     auto* save = static_cast<GtkLabel*>(g_object_get_data(G_OBJECT(item), "save"));
                     auto* remove = static_cast<GtkLabel*>(g_object_get_data(G_OBJECT(item), "remove"));

                     auto* string_object = GTK_STRING_OBJECT(gtk_list_item_get_item(item));

                     g_object_set_data(G_OBJECT(apply), "string-object", string_object);
                     g_object_set_data(G_OBJECT(save), "string-object", string_object);
                     g_object_set_data(G_OBJECT(remove), "string-object", string_object);

                     auto* name = gtk_string_object_get_string(string_object);

                     gtk_label_set_text(label, name);
                   }),
                   self);

  gtk_list_view_set_factory(listview, factory);

  g_object_unref(factory);

  for (const auto& name : get_presets_names()) {
    gtk_string_list_append(string_list, name.c_str());
  }
}

void setup(PresetsMenu* self, app::Application* application) {
  self->data->application = application;
}

void show(GtkWidget* widget) {
  auto* self = FG_PRESETS_MENU(widget);

  auto* active_window = gtk_application_get_active_window(GTK_APPLICATION(self->data->application));

  auto active_window_height = gtk_widget_get_height(GTK_WIDGET(active_window));

  const int menu_height = static_cast<int>(0.5F * static_cast<float>(active_window_height));

  gtk_scrolled_window_set_max_content_height(self->scrolled_window, menu_height);

  GTK_WIDGET_CLASS(presets_menu_parent_class)->show(widget);
}

void dispose(GObject* object) {
  util::debug(log_tag + "disposed"s);

  G_OBJECT_CLASS(presets_menu_parent_class)->dispose(object);
}

void finalize(GObject* object) {
  auto* self = FG_PRESETS_MENU(object);

  g_file_monitor_cancel(self->dir_monitor);

  g_object_unref(self->dir_monitor);

  delete self->data;

  util::debug(log_tag + "finalized"s);

  G_OBJECT_CLASS(presets_menu_parent_class)->finalize(object);
}

void presets_menu_class_init(PresetsMenuClass* klass) {
  auto* object_class = G_OBJECT_CLASS(klass);
  auto* widget_class = GTK_WIDGET_CLASS(klass);

  object_class->dispose = dispose;
  object_class->finalize = finalize;

  widget_class->show = show;

  gtk_widget_class_set_template_from_resource(widget_class, "/com/github/wwmm/fastgame/ui/presets_menu.ui");

  gtk_widget_class_bind_template_child(widget_class, PresetsMenu, string_list);

  gtk_widget_class_bind_template_child(widget_class, PresetsMenu, scrolled_window);
  gtk_widget_class_bind_template_child(widget_class, PresetsMenu, listview);
  gtk_widget_class_bind_template_child(widget_class, PresetsMenu, preset_name);

  gtk_widget_class_bind_template_callback(widget_class, create_preset);
  gtk_widget_class_bind_template_callback(widget_class, import_preset);
}

void presets_menu_init(PresetsMenu* self) {
  gtk_widget_init_template(GTK_WIDGET(self));

  self->data = new Data();

  create_user_directory();

  setup_listview(self, self->listview, self->string_list);

  auto* gfile = g_file_new_for_path(user_presets_dir.c_str());

  self->dir_monitor = g_file_monitor_directory(gfile, G_FILE_MONITOR_NONE, nullptr, nullptr);

  g_object_unref(gfile);

  g_signal_connect(self->dir_monitor, "changed",
                   G_CALLBACK(+[]([[maybe_unused]] GFileMonitor* monitor, GFile* file,
                                  [[maybe_unused]] GFile* other_file, GFileMonitorEvent event_type, PresetsMenu* self) {
                     switch (event_type) {
                       case G_FILE_MONITOR_EVENT_CREATED: {
                         const auto preset_name = util::remove_filename_extension(g_file_get_basename(file));

                         if (preset_name.empty()) {
                           util::warning(log_tag + "can't retrieve information about the preset file"s);

                           return;
                         }

                         for (guint n = 0; n < g_list_model_get_n_items(G_LIST_MODEL(self->string_list)); n++) {
                           if (preset_name == gtk_string_list_get_string(self->string_list, n)) {
                             return;
                           }
                         }

                         gtk_string_list_append(self->string_list, preset_name.c_str());

                         break;
                       }
                       case G_FILE_MONITOR_EVENT_DELETED: {
                         const auto preset_name = util::remove_filename_extension(g_file_get_basename(file));

                         if (preset_name.empty()) {
                           util::warning(log_tag + "can't retrieve information about the preset file"s);

                           return;
                         }

                         for (guint n = 0; n < g_list_model_get_n_items(G_LIST_MODEL(self->string_list)); n++) {
                           if (preset_name == gtk_string_list_get_string(self->string_list, n)) {
                             gtk_string_list_remove(self->string_list, n);

                             return;
                           }
                         }

                         break;
                       }
                       default:
                         break;
                     }
                   }),
                   self);
}

auto create() -> PresetsMenu* {
  return static_cast<PresetsMenu*>(g_object_new(FG_TYPE_PRESETS_MENU, nullptr));
}

}  // namespace ui::presets_menu