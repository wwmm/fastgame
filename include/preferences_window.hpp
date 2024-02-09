#pragma once

// #include "preferences_general.hpp"
#include <adwaita.h>

namespace ui::preferences::window {

G_BEGIN_DECLS

#define EE_TYPE_PREFERENCES_WINDOW (preferences_window_get_type())

G_DECLARE_FINAL_TYPE(PreferencesWindow, preferences_window, EE, PREFERENCES_WINDOW, AdwPreferencesWindow)

G_END_DECLS

auto create() -> PreferencesWindow*;

}  // namespace ui::preferences::window