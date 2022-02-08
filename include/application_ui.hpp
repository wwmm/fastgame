#pragma once

#include <filesystem>

// #include "amdgpu.hpp"
// #include "application.hpp"
// #include "cpu.hpp"
// #include "disk.hpp"
// #include "environment_variables.hpp"
// #include "memory.hpp"
// #include "network.hpp"

// class ApplicationUi : public Gtk::ApplicationWindow {
//  public:
//   ApplicationUi(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder, Application* application);
//   ApplicationUi(const ApplicationUi&) = delete;
//   auto operator=(const ApplicationUi&) -> ApplicationUi& = delete;
//   ApplicationUi(const ApplicationUi&&) = delete;
//   auto operator=(const ApplicationUi&&) -> ApplicationUi& = delete;
//   ~ApplicationUi() override;

//   static auto create(Application* app) -> ApplicationUi*;

//  private:
//   std::string log_tag = "application_ui: ";

//   Application* app;

//   Glib::RefPtr<Gio::Settings> settings;

//   std::filesystem::path user_presets_dir;

//   Gtk::Stack* stack = nullptr;

//   Gtk::Button *add_preset = nullptr, *import_preset = nullptr, *button_apply = nullptr, *button_about = nullptr;

//   Gtk::MenuButton* presets_menu_button = nullptr;

//   Gtk::Entry* preset_name = nullptr;

//   Gtk::ListBox* presets_listbox = nullptr;

//   Gtk::ScrolledWindow* presets_menu_scrolled_window = nullptr;

//   Gtk::Spinner* headerbar_spinner = nullptr;

//   Gtk::Entry* game_executable = nullptr;

//   Gtk::Switch* use_dark_theme = nullptr;

//   EnvironmentVariables* environment_variables = nullptr;

//   Cpu* cpu = nullptr;

//   Amdgpu* amdgpu = nullptr;

//   Memory* memory = nullptr;

//   Disk* disk = nullptr;

//   Network* network = nullptr;

//   std::vector<sigc::connection> connections;

//   static void get_object(const Glib::RefPtr<Gtk::Builder>& builder,
//                          const std::string& name,
//                          Glib::RefPtr<Gtk::Adjustment>& object) {
//     object = Glib::RefPtr<Gtk::Adjustment>::cast_dynamic(builder->get_object(name));
//   }

//   void create_user_directory();

//   auto get_presets_names() -> std::vector<std::string>;

//   void create_preset();

//   void save_preset(const std::string& name, const std::filesystem::path& directory);

//   void load_preset(const std::string& name);

//   void import_preset_file();

//   static auto on_listbox_sort(Gtk::ListBoxRow* row1, Gtk::ListBoxRow* row2) -> int;

//   void populate_listbox();

//   void on_presets_menu_button_clicked();

//   void apply_settings();
// };
