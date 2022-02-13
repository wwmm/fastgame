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

}  // namespace ui