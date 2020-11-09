#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <gtkmm/adjustment.h>
#include <gtkmm/builder.h>
#include <gtkmm/grid.h>
#include <gtkmm/stack.h>
#include <gtkmm/switch.h>

class Network : public Gtk::Grid {
 public:
  Network(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder);
  Network(const Network&) = delete;
  auto operator=(const Network&) -> Network& = delete;
  Network(const Network&&) = delete;
  auto operator=(const Network &&) -> Network& = delete;
  ~Network() override;

  static auto add_to_stack(Gtk::Stack* stack) -> Network*;

  auto get_use_tcp_sack() -> bool;

  void set_use_tcp_sack(const bool& state);

  auto get_tcp_keepalive_time() -> int;

  void set_tcp_keepalive_time(const int& value);

  auto get_tcp_keepalive_interval() -> int;

  void set_tcp_keepalive_interval(const int& value);

 private:
  std::string log_tag = "network: ";

  Gtk::Switch* use_tcp_sack = nullptr;

  Glib::RefPtr<Gtk::Adjustment> tcp_keepalive_time, tcp_keepalive_interval;
};

#endif
