#include "netlink.hpp"
#include <bits/types/struct_iovec.h>
#include <linux/connector.h>
#include <linux/limits.h>
#include <linux/netlink.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "netlink_c.h"

/*
  based on https://gist.github.com/L-P/9487407 and
  https://nick-black.com/dankwiki/index.php/The_Proc_Connector_and_Socket_Filters
*/

namespace {

namespace fs = std::filesystem;

}  // namespace

Netlink::Netlink() {
  connect();
  subscribe();
}

Netlink::~Netlink() {
  listen = false;

  close(nl_socket);
}

void Netlink::connect() {
  nl_socket = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_CONNECTOR);

  if (nl_socket == -1) {
    listen = false;

    std::cout << log_tag + "socket creation failed!" << '\n';
  } else {
    std::cout << log_tag + "socket created" << '\n';

    sockaddr_nl addr{};

    addr.nl_family = AF_NETLINK;
    addr.nl_groups = CN_IDX_PROC;
    addr.nl_pid = getpid();

    auto rc = bind(nl_socket, (sockaddr*)&addr, sizeof(addr));

    if (rc == -1) {
      listen = false;

      std::cout << log_tag + "socket binding failed!" << '\n';
    } else {
      std::cout << log_tag + "socket binding succesful!" << '\n';
    }
  }
}

void Netlink::subscribe() {
  struct iovec iov[3];

  prepare_iovec(iov);

  if (writev(nl_socket, iov, 3) == -1) {
    std::cout << log_tag + "failed to send PROC_CN_MCAST_LISTEN!" << '\n';
  } else {
    std::cout << log_tag + "sent PROC_CN_MCAST_LISTEN to kernel" << '\n';
  }
}

void Netlink::handle_events() {
  std::vector<char> buff(getpagesize());
  sockaddr_nl addr{};
  iovec iov[1];

  iov[0].iov_base = buff.data();
  iov[0].iov_len = sizeof(char) * buff.size();

  msghdr msg_hdr{.msg_name = &addr,
                 .msg_namelen = sizeof(addr),
                 .msg_iov = iov,
                 .msg_iovlen = 1,
                 .msg_control = nullptr,
                 .msg_controllen = 0,
                 .msg_flags = 0};

  while (listen) {
    auto len = recvmsg(nl_socket, &msg_hdr, 0);

    if (len == -1) {
      continue;
    }

    if (addr.nl_pid != 0) {  // https://linux.die.net/man/7/netlink
      continue;
    }

    auto* nlmsg_hdr = reinterpret_cast<nlmsghdr*>(buff.data());

    while (NLMSG_OK(nlmsg_hdr, len)) {
      if (nlmsg_hdr->nlmsg_type == NLMSG_NOOP) {
        continue;
      }

      if ((nlmsg_hdr->nlmsg_type == NLMSG_ERROR) || (nlmsg_hdr->nlmsg_type == NLMSG_OVERRUN)) {
        break;
      }

      auto* cnmsg = static_cast<cn_msg*>(NLMSG_DATA(nlmsg_hdr));

      handle_msg(cnmsg);

      if (nlmsg_hdr->nlmsg_type == NLMSG_DONE) {
        break;
      }

      nlmsg_hdr = NLMSG_NEXT(nlmsg_hdr, len);
    }
  }
}

void Netlink::handle_msg(cn_msg* msg) {
  if ((msg->id.idx != CN_IDX_PROC) || (msg->id.val != CN_VAL_PROC)) {
    return;
  }

  struct fg_cn_msg parsed_msg = parse_cn_msg(msg);

  int pid = 0;
  std::string comm;
  std::string cmdline;
  std::string exe_path;
  std::string child_comm;

  switch (parsed_msg.event) {
    case FG_PROC_EVENT_FORK:
      child_comm = get_comm(parsed_msg.child_pid);

      if (!child_comm.empty()) {
        new_fork(parsed_msg.child_tgid, parsed_msg.child_pid, child_comm);
      }

      break;
    case FG_PROC_EVENT_EXEC:
      pid = parsed_msg.process_pid;

      comm = get_comm(pid);
      cmdline = get_cmdline(pid);
      exe_path = get_exe_path(pid);

      if (!comm.empty() && !cmdline.empty()) {
        new_exec(pid, comm, cmdline, exe_path);
      }

      break;
    case FG_PROC_EVENT_COMM:
      pid = parsed_msg.process_pid;

      comm = get_comm(pid);
      cmdline = get_cmdline(pid);
      exe_path = get_exe_path(pid);

      if (!comm.empty() && !cmdline.empty()) {
        new_exec(pid, comm, cmdline, exe_path);
      }

      break;
    case FG_PROC_EVENT_EXIT:
      new_exit(parsed_msg.process_pid);

      break;
    default:
      break;
  }
}

auto Netlink::get_comm(const int& pid) -> std::string {
  auto path = fs::path("/proc/" + std::to_string(pid) + "/comm");

  try {
    if (fs::is_regular_file(path)) {
      std::ostringstream stream;

      stream << std::ifstream(path).rdbuf();

      auto out = stream.str();

      out.erase(out.length() - 1);  // remove carriage return

      return out;
    }

    return "";
  } catch (std::exception& e) {
    // std::cout << log_tag + e.what() << std::endl;

    return "";
  }
}

auto Netlink::get_cmdline(const int& pid) -> std::string {
  auto path = fs::path("/proc/" + std::to_string(pid) + "/cmdline");

  try {
    if (fs::is_regular_file(path)) {
      std::ostringstream stream;

      stream << std::ifstream(path).rdbuf();

      auto out = stream.str();

      out.erase(out.length() - 1);  // remove carriage return

      return out;
    }

    return "";
  } catch (std::exception& e) {
    // std::cout << log_tag + e.what() << std::endl;

    return "";
  }
}

auto Netlink::get_exe_path(const int& pid) -> std::string {
  char buff[PATH_MAX];
  auto path = fs::path("/proc/" + std::to_string(pid) + "/exe");

  ssize_t len = ::readlink(path.c_str(), buff, sizeof(buff) - 1);

  if (len != -1) {
    buff[len] = '\0';

    return {buff};
  }

  return "";
}