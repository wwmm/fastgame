#pragma once

#include <adwaita.h>
#include "application.hpp"

namespace ui::presets_menu {

G_BEGIN_DECLS

#define FG_TYPE_PRESETS_MENU (presets_menu_get_type())

G_DECLARE_FINAL_TYPE(PresetsMenu, presets_menu, FG, PRESETS_MENU, GtkPopover)

G_END_DECLS

auto create() -> PresetsMenu*;

void setup(PresetsMenu* self, app::Application* application);

}  // namespace ui::presets_menu