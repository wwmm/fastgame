#pragma once

#include <adwaita.h>

namespace ui::application_window {

G_BEGIN_DECLS

#define FG_TYPE_APPLICATION_WINDOW (application_window_get_type())

G_DECLARE_FINAL_TYPE(ApplicationWindow, application_window, FG, APP_WINDOW, AdwApplicationWindow)

G_END_DECLS

auto create(GApplication* gapp) -> ApplicationWindow*;

}  // namespace ui::application_window
