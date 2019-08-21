#ifndef NETLINK_HPP
#define NETLINK_HPP

#include <linux/connector.h>
#include <linux/netlink.h>
#include <boost/signals2.hpp>
#include <iostream>

class Netlink {
 public:
  Netlink();

  virtual ~Netlink();

  bool listen = true;

  boost::signals2::signal<void(int, std::string)> new_exec;

  void handle_events();

 private:
  std::string log_tag = "netlink: ";

  int nl_socket;

  void connect();
  void subscribe();
  std::string get_process_name(const int& pid);
};

#endif