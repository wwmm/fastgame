#include "application_ui.hpp"
#include <glibmm/i18n.h>
#include <glibmm/miscutils.h>
#include <gtkmm/dialog.h>
#include <gtkmm/filechoosernative.h>
#include <gtkmm/icontheme.h>
#include <gtkmm/settings.h>
#include <filesystem>
#include "util.hpp"

ApplicationUi::ApplicationUi(BaseObjectType* cobject,
                             const Glib::RefPtr<Gtk::Builder>& builder,
                             Application* application)
    : Gtk::ApplicationWindow(cobject),
      app(application),
      settings(app->settings),
      user_presets_dir(Glib::get_user_config_dir() + "/fastgame") {
  Gtk::IconTheme::get_default()->add_resource_path("/com/github/wwmm/fastgame/icons");

  // loading glade widgets

  builder->get_widget("stack", stack);
  builder->get_widget("add_preset", add_preset);
  builder->get_widget("import_preset", import_preset);
  builder->get_widget("preset_name", preset_name);
  builder->get_widget("presets_listbox", presets_listbox);
  builder->get_widget("presets_menu_button", presets_menu_button);
  builder->get_widget("presets_menu_scrolled_window", presets_menu_scrolled_window);

  create_user_directory();

  environment_variables = EnvironmentVariables::add_to_stack(stack);
  cpu = Cpu::add_to_stack(stack);
  amdgpu = Amdgpu::add_to_stack(stack);
  memory = Memory::add_to_stack(stack);

  stack->connect_property_changed("visible-child",
                                  sigc::mem_fun(*this, &ApplicationUi::on_stack_visible_child_changed));

  // binding glade widgets to gsettings keys

  auto flag = Gio::SettingsBindFlags::SETTINGS_BIND_DEFAULT;

  settings->bind("use-dark-theme", Gtk::Settings::get_default().get(), "gtk_application_prefer_dark_theme", flag);

  // signal connection

  add_preset->signal_clicked().connect([=]() { create_preset(); });

  import_preset->signal_clicked().connect([=]() { import_preset_file(); });

  presets_listbox->set_sort_func(sigc::ptr_fun(&ApplicationUi::on_listbox_sort));

  presets_menu_button->signal_clicked().connect(sigc::mem_fun(*this, &ApplicationUi::on_presets_menu_button_clicked));

  // restore the window size

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

void ApplicationUi::create_user_directory() {
  if (!std::filesystem::is_directory(user_presets_dir)) {
    if (std::filesystem::create_directories(user_presets_dir)) {
      util::debug(log_tag + "user presets directory created: " + user_presets_dir.string());
    } else {
      util::warning(log_tag + "failed to create user presets directory: " + user_presets_dir.string());
    }
  } else {
    util::debug(log_tag + "user presets directory already exists: " + user_presets_dir.string());
  }
}

auto ApplicationUi::get_presets_names() -> std::vector<std::string> {
  std::vector<std::string> names;

  for (const auto& entry : std::filesystem::directory_iterator(user_presets_dir)) {
    names.emplace_back(entry.path().stem().string());
  }

  return names;
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

void ApplicationUi::create_preset() {
  std::string name = preset_name->get_text();

  if (!name.empty()) {
    std::string illegalChars = "\\/";

    for (auto it = name.begin(); it < name.end(); ++it) {
      bool found = illegalChars.find(*it) != std::string::npos;

      if (found) {
        preset_name->set_text("");

        return;
      }
    }

    preset_name->set_text("");

    // app->presets_manager->add(preset_type, name);

    // populate_listbox(preset_type);
  }
}

void ApplicationUi::import_preset_file() {
  auto* main_window = dynamic_cast<Gtk::Window*>(this->get_toplevel());

  auto dialog = Gtk::FileChooserNative::create(
      _("Import Presets"), *main_window, Gtk::FileChooserAction::FILE_CHOOSER_ACTION_OPEN, _("Open"), _("Cancel"));

  auto dialog_filter = Gtk::FileFilter::create();

  dialog_filter->set_name(_("Presets"));
  dialog_filter->add_pattern("*.json");

  dialog->add_filter(dialog_filter);

  dialog->signal_response().connect([=](auto response_id) {
    switch (response_id) {
      case Gtk::ResponseType::RESPONSE_ACCEPT: {
        for (const auto& file_path : dialog->get_filenames()) {
          // app->presets_manager->import(preset_type, file_path);
        }

        // populate_listbox(preset_type);

        break;
      }
      default:
        break;
    }
  });

  dialog->set_modal(true);
  dialog->set_select_multiple(true);
  dialog->show();
}

auto ApplicationUi::on_listbox_sort(Gtk::ListBoxRow* row1, Gtk::ListBoxRow* row2) -> int {
  auto name1 = row1->get_name();
  auto name2 = row2->get_name();

  std::vector<std::string> names = {name1, name2};

  std::sort(names.begin(), names.end());

  if (name1 == names[0]) {
    return -1;
  }
  if (name2 == names[0]) {
    return 1;
  }

  return 0;
}

void ApplicationUi::populate_listbox() {
  auto children = presets_listbox->get_children();

  for (const auto& c : children) {
    presets_listbox->remove(*c);
  }

  auto names = get_presets_names();

  for (const auto& name : names) {
    auto b = Gtk::Builder::create_from_resource("/com/github/wwmm/fastgame/ui/preset_row.glade");

    Gtk::ListBoxRow* row = nullptr;
    Gtk::Button* apply_btn = nullptr;
    Gtk::Button* save_btn = nullptr;
    Gtk::Button* remove_btn = nullptr;
    Gtk::Label* label = nullptr;

    b->get_widget("preset_row", row);
    b->get_widget("apply", apply_btn);
    b->get_widget("save", save_btn);
    b->get_widget("remove", remove_btn);
    b->get_widget("name", label);

    row->set_name(name);

    label->set_text(name);

    connections.emplace_back(apply_btn->signal_clicked().connect([=]() {
      // app->presets_manager->load(preset_type, row->get_name());
    }));

    connections.emplace_back(save_btn->signal_clicked().connect([=]() {
      // app->presets_manager->save(preset_type, name);
    }));

    connections.emplace_back(remove_btn->signal_clicked().connect([=]() {
      // app->presets_manager->remove(preset_type, name);

      populate_listbox();
    }));

    presets_listbox->add(*row);
    presets_listbox->show_all();
  }
}

void ApplicationUi::on_presets_menu_button_clicked() {
  auto* parent = dynamic_cast<Gtk::ApplicationWindow*>(this->get_toplevel());
  const float scaling_factor = 0.7F;

  int height = static_cast<int>(scaling_factor * static_cast<float>(parent->get_allocated_height()));

  presets_menu_scrolled_window->set_max_content_height(height);

  populate_listbox();
}