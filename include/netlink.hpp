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

  std::string get_comm(const int& pid);

  boost::signals2::signal<void(int, std::string, std::string, std::string)> new_exec;
  boost::signals2::signal<void(int, int, std::string)> new_fork;
  boost::signals2::signal<void(int)> new_exit;

  void handle_events();

 private:
  std::string log_tag = "netlink: ";

  int nl_socket;

  void connect();
  void subscribe();

  std::string get_cmdline(const int& pid);
  std::string get_exe_path(const int& pid);
};

#endif