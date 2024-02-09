#include "preferences_general.hpp"
#include "ui_helpers.hpp"
#include "util.hpp"

namespace ui::preferences::general {

using namespace std::string_literals;

auto constexpr log_tag = "preferences_general: ";

struct _PreferencesGeneral {
  AdwPreferencesPage parent_instance;

  GtkSwitch *enable_autostart, *process_all_inputs, *process_all_outputs, *theme_switch, *shutdown_on_window_close,
      *use_cubic_volumes, *autohide_popovers, *reset_volume_on_startup;

  GSettings* settings;
};

G_DEFINE_TYPE(PreferencesGeneral, preferences_general, ADW_TYPE_PREFERENCES_PAGE)

void dispose(GObject* object) {
  auto* self = EE_PREFERENCES_GENERAL(object);

  g_object_unref(self->settings);

  util::debug(log_tag + "disposed"s);

  G_OBJECT_CLASS(preferences_general_parent_class)->dispose(object);
}

void preferences_general_class_init(PreferencesGeneralClass* klass) {
  auto* object_class = G_OBJECT_CLASS(klass);
  auto* widget_class = GTK_WIDGET_CLASS(klass);

  object_class->dispose = dispose;

  gtk_widget_class_set_template_from_resource(widget_class, "/com/github/wwmm/fastgame/ui/preferences_general.ui");

  gtk_widget_class_bind_template_child(widget_class, PreferencesGeneral, theme_switch);
  gtk_widget_class_bind_template_child(widget_class, PreferencesGeneral, autohide_popovers);
}

void preferences_general_init(PreferencesGeneral* self) {
  gtk_widget_init_template(GTK_WIDGET(self));

  self->settings = g_settings_new("com.github.wwmm.fastgame");

  // initializing some widgets

  gsettings_bind_widgets<"use-dark-theme", "autohide-popovers">(self->settings, self->theme_switch,
                                                                self->autohide_popovers);
}

auto create() -> PreferencesGeneral* {
  return static_cast<PreferencesGeneral*>(g_object_new(EE_TYPE_PREFERENCES_GENERAL, nullptr));
}

}  // namespace ui::preferences::general