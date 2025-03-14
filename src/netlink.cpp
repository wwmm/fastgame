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
#include "util.hpp"

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
  // SOCK_NONBLOCK makes the cpu cores usage to be very high because the loop iterates too often

  nl_socket = socket(PF_NETLINK, SOCK_DGRAM | SOCK_CLOEXEC, NETLINK_CONNECTOR);

  if (nl_socket == -1) {
    listen = false;

    util::debug("socket creation failed!");
  } else {
    util::debug("socket created");

    sockaddr_nl addr{};

    addr.nl_family = AF_NETLINK;
    addr.nl_groups = CN_IDX_PROC;
    addr.nl_pid = getpid();

    auto rc = bind(nl_socket, (sockaddr*)&addr, sizeof(addr));

    if (rc == -1) {
      listen = false;

      util::debug("socket binding failed!");
    } else {
      util::debug("socket binding succesful!");
    }
  }
}

void Netlink::subscribe() const {
  if (nl_subscribe(nl_socket) == -1) {
    util::debug("failed to send PROC_CN_MCAST_LISTEN!");
  } else {
    util::debug("sent PROC_CN_MCAST_LISTEN to kernel");
  }
}

void Netlink::handle_events() {
  std::vector<char> buff(getpagesize());
  sockaddr_nl addr{};
  iovec iov[1];

  msghdr msg_hdr{.msg_name = &addr,
                 .msg_namelen = sizeof(addr),
                 .msg_iov = iov,
                 .msg_iovlen = 1,
                 .msg_control = nullptr,
                 .msg_controllen = 0,
                 .msg_flags = 0};

  iov[0].iov_base = buff.data();
  iov[0].iov_len = sizeof(char) * buff.size();

  auto input_file = std::filesystem::temp_directory_path() / std::filesystem::path{"fastgame.json"};

  while (listen) {
    if (!std::filesystem::is_regular_file(input_file)) {
      return;
    }

    auto len = recvmsg(nl_socket, &msg_hdr, 0);

    if (len == -1) {
      continue;
    }

    if (addr.nl_pid != 0) {  // https://linux.die.net/man/7/netlink
      continue;
    }

    auto* nlmsg_hdr = reinterpret_cast<nlmsghdr*>(buff.data());

    while (NLMSG_OK(nlmsg_hdr, len)) {
      if (!std::filesystem::is_regular_file(input_file)) {
        return;
      }

      if ((nlmsg_hdr->nlmsg_type == NLMSG_ERROR) || (nlmsg_hdr->nlmsg_type == NLMSG_NOOP)) {
        continue;
      }

      auto* cnmsg = static_cast<cn_msg*>(NLMSG_DATA(nlmsg_hdr));

      if (!handle_msg(cnmsg)) {
        continue;
      }

      if (nlmsg_hdr->nlmsg_type == NLMSG_DONE) {
        break;
      }

      nlmsg_hdr = NLMSG_NEXT(nlmsg_hdr, len);
    }
  }
}

auto Netlink::handle_msg(cn_msg* msg) -> bool {
  if ((msg->id.idx != CN_IDX_PROC) || (msg->id.val != CN_VAL_PROC)) {
    return false;
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

  return true;
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