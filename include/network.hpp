#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <gtkmm/adjustment.h>
#include <gtkmm/builder.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/grid.h>
#include <gtkmm/stack.h>
#include <gtkmm/switch.h>
#include "gtkmm/comboboxtext.h"

class Network : public Gtk::Grid {
 public:
  Network(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder);
  Network(const Network&) = delete;
  auto operator=(const Network&) -> Network& = delete;
  Network(const Network&&) = delete;
  auto operator=(const Network &&) -> Network& = delete;
  ~Network() override;

  static auto add_to_stack(Gtk::Stack* stack) -> Network*;

  auto get_use_tcp_mtu_probing() -> bool;

  void set_use_tcp_mtu_probing(const bool& state);

  auto get_tcp_max_reordering() -> int;

  void set_tcp_max_reordering(const int& value);

  auto get_tcp_probe_interval() -> int;

  void set_tcp_probe_interval(const int& value);

  auto get_tcp_congestion_control() -> std::string;

  void set_tcp_congestion_control(const std::string& value);

 private:
  std::string log_tag = "network: ";

  Gtk::Switch* use_tcp_mtu_probing = nullptr;

  Gtk::ComboBoxText* tcp_congestion_control = nullptr;

  Glib::RefPtr<Gtk::Adjustment> tcp_max_reordering, tcp_probe_interval;
};

#endif
