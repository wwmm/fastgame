#pragma once

#include <fcntl.h>
#include <linux/connector.h>
#include <sys/uio.h>
#include <boost/signals2.hpp>
#include <boost/signals2/variadic_signal.hpp>
#include <string>

class Netlink {
 public:
  Netlink();
  Netlink(const Netlink&) = delete;
  auto operator=(const Netlink&) -> Netlink& = delete;
  Netlink(const Netlink&&) = delete;
  auto operator=(const Netlink&&) -> Netlink& = delete;

  virtual ~Netlink();

  bool listen = true;

  static auto get_comm(const int& pid) -> std::string;

  boost::signals2::signal<void(int, std::string, std::string, std::string)> new_exec;
  boost::signals2::signal<void(int, int, std::string)> new_fork;
  boost::signals2::signal<void(int)> new_exit;

  void handle_events();

 private:
  std::string log_tag = "netlink: ";

  int nl_socket = 0;

  void connect();
  void subscribe() const;
  void handle_msg(cn_msg* msg);

  static auto get_cmdline(const int& pid) -> std::string;
  static auto get_exe_path(const int& pid) -> std::string;
};