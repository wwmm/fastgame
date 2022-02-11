#pragma once

#include <adwaita.h>
#include <filesystem>
#include "cpu.hpp"
#include "environment_variables.hpp"
#include "presets_menu.hpp"

#include <boost/process.hpp>
#include <boost/process/search_path.hpp>
#include <boost/process/spawn.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

namespace ui::application_window {

G_BEGIN_DECLS

#define FG_TYPE_APPLICATION_WINDOW (application_window_get_type())

G_DECLARE_FINAL_TYPE(ApplicationWindow, application_window, FG, APP_WINDOW, AdwApplicationWindow)

G_END_DECLS

auto create(GApplication* gapp) -> ApplicationWindow*;

}  // namespace ui::application_window

// #include "amdgpu.hpp"
// #include "application.hpp"
// #include "disk.hpp"
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

//   Gtk::Switch* use_dark_theme = nullptr;

//   Cpu* cpu = nullptr;

//   Amdgpu* amdgpu = nullptr;

//   Memory* memory = nullptr;

//   Disk* disk = nullptr;

//   Network* network = nullptr;

//   std::vector<sigc::connection> connections;

//   void create_preset();

//   void save_preset(const std::string& name, const std::filesystem::path& directory);

//   void load_preset(const std::string& name);

//   void populate_listbox();

//   void on_presets_menu_button_clicked();
// };
