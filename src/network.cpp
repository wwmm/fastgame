#include "network.hpp"
#include <glibmm/i18n.h>
#include <filesystem>
#include <fstream>
#include <string>
#include "util.hpp"

namespace fs = std::filesystem;

Network::Network(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder) : Gtk::Grid(cobject) {
  // loading glade widgets

  builder->get_widget("use_tcp_mtu_probing", use_tcp_mtu_probing);
  builder->get_widget("tcp_congestion_control", tcp_congestion_control);

  tcp_max_reordering = Glib::RefPtr<Gtk::Adjustment>::cast_dynamic(builder->get_object("tcp_max_reordering"));
  tcp_probe_interval = Glib::RefPtr<Gtk::Adjustment>::cast_dynamic(builder->get_object("tcp_probe_interval"));

  // initializing widgets

  auto tcp_congestion_control_list = util::read_system_setting("/proc/sys/net/ipv4/tcp_available_congestion_control");

  auto tcp_congestion_control_value = util::read_system_setting("/proc/sys/net/ipv4/tcp_congestion_control")[0];

  util::debug(log_tag + "tcp congestion control: " + tcp_congestion_control_value);

  for (auto& value : tcp_congestion_control_list) {
    if (value.find('[') != std::string::npos) {
      value = value.erase(0, 1).erase(value.size() - 1, 1);  // removing the [] characters
    }

    tcp_congestion_control->append(value);
  }

  tcp_congestion_control->set_active_text(tcp_congestion_control_value);

  use_tcp_mtu_probing->set_active(
      static_cast<bool>(std::stoi(util::read_system_setting("/proc/sys/net/ipv4/tcp_mtu_probing")[0])));

  tcp_max_reordering->set_value(
      static_cast<int>(std::stoi(util::read_system_setting("/proc/sys/net/ipv4/tcp_max_reordering")[0])));

  tcp_probe_interval->set_value(
      static_cast<int>(std::stoi(util::read_system_setting("/proc/sys/net/ipv4/tcp_probe_interval")[0])));
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

auto Network::get_use_tcp_mtu_probing() -> bool {
  return use_tcp_mtu_probing->get_active();
}

void Network::set_use_tcp_mtu_probing(const bool& state) {
  use_tcp_mtu_probing->set_active(state);
}

auto Network::get_tcp_max_reordering() -> int {
  return static_cast<int>(tcp_max_reordering->get_value());
}

void Network::set_tcp_max_reordering(const int& value) {
  tcp_max_reordering->set_value(value);
}

auto Network::get_tcp_probe_interval() -> int {
  return static_cast<int>(tcp_probe_interval->get_value());
}

void Network::set_tcp_probe_interval(const int& value) {
  tcp_probe_interval->set_value(value);
}

auto Network::get_tcp_congestion_control() -> std::string {
  return tcp_congestion_control->get_active_text();
}

void Network::set_tcp_congestion_control(const std::string& value) {
  tcp_congestion_control->set_active_text(value);
}