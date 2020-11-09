#include "network.hpp"
#include <glibmm/i18n.h>
#include <filesystem>
#include <fstream>
#include <string>
#include "util.hpp"

namespace fs = std::filesystem;

Network::Network(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder) : Gtk::Grid(cobject) {
  // loading glade widgets

  builder->get_widget("use_tcp_sack", use_tcp_sack);

  tcp_keepalive_time = Glib::RefPtr<Gtk::Adjustment>::cast_dynamic(builder->get_object("tcp_keepalive_time"));
  tcp_keepalive_interval = Glib::RefPtr<Gtk::Adjustment>::cast_dynamic(builder->get_object("tcp_keepalive_interval"));

  // initializing widgets

  use_tcp_sack->set_active(static_cast<bool>(std::stoi(util::read_system_setting("/proc/sys/net/ipv4/tcp_sack")[0])));

  tcp_keepalive_time->set_value(
      static_cast<int>(std::stoi(util::read_system_setting("/proc/sys/net/ipv4/tcp_keepalive_time")[0])));

  tcp_keepalive_interval->set_value(
      static_cast<int>(std::stoi(util::read_system_setting("/proc/sys/net/ipv4/tcp_keepalive_intvl")[0])));
}

Network::~Network() {
  util::debug(log_tag + "destroyed");
}

auto Network::add_to_stack(Gtk::Stack* stack) -> Network* {
  auto builder = Gtk::Builder::create_from_resource("/com/github/wwmm/fastgame/ui/network.glade");

  Network* ui = nullptr;

  builder->get_widget_derived("widgets_grid", ui);

  stack->add(*ui, "network", _("Network"));

  return ui;
}

auto Network::get_use_tcp_sack() -> bool {
  return use_tcp_sack->get_active();
}

void Network::set_use_tcp_sack(const bool& state) {
  use_tcp_sack->set_active(state);
}

auto Network::get_tcp_keepalive_time() -> int {
  return static_cast<int>(tcp_keepalive_time->get_value());
}

void Network::set_tcp_keepalive_time(const int& value) {
  tcp_keepalive_time->set_value(value);
}

auto Network::get_tcp_keepalive_interval() -> int {
  return static_cast<int>(tcp_keepalive_interval->get_value());
}

void Network::set_tcp_keepalive_interval(const int& value) {
  tcp_keepalive_interval->set_value(value);
}