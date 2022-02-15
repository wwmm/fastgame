#include "preferences_window.hpp"

namespace ui::preferences::window {

using namespace std::string_literals;

auto constexpr log_tag = "preferences_window: ";

struct _PreferencesWindow {
  AdwPreferencesWindow parent_instance;

  ui::preferences::general::PreferencesGeneral* page_general;
};

G_DEFINE_TYPE(PreferencesWindow, preferences_window, ADW_TYPE_PREFERENCES_WINDOW)

void dispose(GObject* object) {
  auto* self = EE_PREFERENCES_WINDOW(object);

  adw_preferences_window_remove(ADW_PREFERENCES_WINDOW(self), ADW_PREFERENCES_PAGE(self->page_general));

  util::debug(log_tag + "disposed"s);

  G_OBJECT_CLASS(preferences_window_parent_class)->dispose(object);
}

void preferences_window_class_init(PreferencesWindowClass* klass) {
  auto* object_class = G_OBJECT_CLASS(klass);
  auto* widget_class = GTK_WIDGET_CLASS(klass);

  object_class->dispose = dispose;

  gtk_widget_class_set_template_from_resource(widget_class, "/com/github/wwmm/fastgame/ui/preferences_window.ui");
}

void preferences_window_init(PreferencesWindow* self) {
  gtk_widget_init_template(GTK_WIDGET(self));

  self->page_general = ui::preferences::general::create();

  adw_preferences_window_add(ADW_PREFERENCES_WINDOW(self), ADW_PREFERENCES_PAGE(self->page_general));
}

auto create() -> PreferencesWindow* {
  return static_cast<PreferencesWindow*>(g_object_new(EE_TYPE_PREFERENCES_WINDOW, nullptr));
}

}  // namespace ui::preferences::window