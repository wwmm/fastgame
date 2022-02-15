#pragma once

#include <adwaita.h>
#include <fmt/core.h>
#include <sstream>
#include "string_literal_wrapper.hpp"

namespace ui {

auto parse_spinbutton_output(GtkSpinButton* button, const char* unit) -> bool;

auto parse_spinbutton_input(GtkSpinButton* button, double* new_value) -> int;

template <StringLiteralWrapper sl_wrapper>
void prepare_spinbutton(GtkSpinButton* button) {
  g_signal_connect(button, "output", G_CALLBACK(+[](GtkSpinButton* button, gpointer user_data) {
                     return parse_spinbutton_output(button, sl_wrapper.msg.data());
                   }),
                   nullptr);

  g_signal_connect(button, "input", G_CALLBACK(+[](GtkSpinButton* button, gdouble* new_value, gpointer user_data) {
                     return parse_spinbutton_input(button, new_value);
                   }),
                   nullptr);
}

template <StringLiteralWrapper key_wrapper, typename... Targs>
void prepare_spinbuttons(Targs... button) {
  (prepare_spinbutton<key_wrapper>(button), ...);
}

template <typename T>
void gsettings_bind_widget(GSettings* settings,
                           const char* key,
                           T widget,
                           GSettingsBindFlags flags = G_SETTINGS_BIND_DEFAULT) {
  static_assert(std::is_same_v<T, GtkSpinButton*> || std::is_same_v<T, GtkToggleButton*> ||
                std::is_same_v<T, GtkSwitch*> || std::is_same_v<T, GtkComboBoxText*> || std::is_same_v<T, GtkScale*>);

  if constexpr (std::is_same_v<T, GtkSpinButton*>) {
    g_settings_bind(settings, key, gtk_spin_button_get_adjustment(widget), "value", flags);
  }

  if constexpr (std::is_same_v<T, GtkScale*>) {
    g_settings_bind(settings, key, gtk_range_get_adjustment(GTK_RANGE(widget)), "value", flags);
  }

  if constexpr (std::is_same_v<T, GtkToggleButton*> || std::is_same_v<T, GtkSwitch*>) {
    g_settings_bind(settings, key, widget, "active", flags);
  }

  if constexpr (std::is_same_v<T, GtkComboBoxText*>) {
    g_settings_bind(settings, key, widget, "active-id", flags);
  }
}

template <StringLiteralWrapper... key_wrapper, typename... Targs>
void gsettings_bind_widgets(GSettings* settings, Targs... widget) {
  (gsettings_bind_widget(settings, key_wrapper.msg.data(), widget), ...);
}

}  // namespace ui