#pragma once

#include <adwaita.h>
#include <filesystem>
#include "cpu.hpp"
#include "environment_variables.hpp"
#include "memory.hpp"
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
// #include "network.hpp"

//   Gtk::Switch* use_dark_theme = nullptr;

//   Amdgpu* amdgpu = nullptr;

//   Disk* disk = nullptr;

//   Network* network = nullptr;