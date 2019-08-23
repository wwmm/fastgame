#include "netlink.hpp"
#include <linux/cn_proc.h>
#include <sys/socket.h>
#include <unistd.h>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

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

    std::cout << log_tag + "socket creation failed!" << std::endl;
  } else {
    std::cout << log_tag + "socket created" << std::endl;

    struct sockaddr_nl sa_nl;

    sa_nl.nl_family = AF_NETLINK;
    sa_nl.nl_groups = CN_IDX_PROC;
    sa_nl.nl_pid = getpid();

    int rc;

    rc = bind(nl_socket, (struct sockaddr*)&sa_nl, sizeof(sa_nl));

    if (rc == -1) {
      listen = false;

      std::cout << log_tag + "socket binding failed!" << std::endl;
    } else {
      std::cout << log_tag + "socket binding succesful!" << std::endl;
    }
  }
}

void Netlink::subscribe() {
  struct __attribute__((aligned(NLMSG_ALIGNTO))) {
    struct nlmsghdr nl_hdr;
    struct __attribute__((__packed__)) {
      struct cn_msg cn_msg;
      enum proc_cn_mcast_op cn_mcast;
    };
  } nlcn_msg;

  memset(&nlcn_msg, 0, sizeof(nlcn_msg));

  nlcn_msg.nl_hdr.nlmsg_len = sizeof(nlcn_msg);
  nlcn_msg.nl_hdr.nlmsg_pid = getpid();
  nlcn_msg.nl_hdr.nlmsg_type = NLMSG_DONE;

  nlcn_msg.cn_msg.id.idx = CN_IDX_PROC;
  nlcn_msg.cn_msg.id.val = CN_VAL_PROC;
  nlcn_msg.cn_msg.len = sizeof(enum proc_cn_mcast_op);

  nlcn_msg.cn_mcast = PROC_CN_MCAST_LISTEN;

  int rc = send(nl_socket, &nlcn_msg, sizeof(nlcn_msg), 0);

  if (rc == -1) {
    listen = false;

    std::cout << log_tag + "failed to subscribe failed!" << std::endl;
  }
}

void Netlink::handle_events() {
  struct __attribute__((aligned(NLMSG_ALIGNTO))) {
    struct nlmsghdr nl_hdr;
    struct __attribute__((__packed__)) {
      struct cn_msg cn_msg;
      struct proc_event proc_ev;
    };
  } nlcn_msg;

  while (listen) {
    recv(nl_socket, &nlcn_msg, sizeof(nlcn_msg), 0);

    std::string name, cmdline;

    switch (nlcn_msg.proc_ev.what) {
      case proc_event::PROC_EVENT_FORK:
        // process_event(nlcn_msg.proc_ev.event_data.fork.child_pid);

        break;
      case proc_event::PROC_EVENT_EXEC:
        process_event(nlcn_msg.proc_ev.event_data.exec.process_pid);

        break;
      case proc_event::PROC_EVENT_COMM:
        process_event(nlcn_msg.proc_ev.event_data.comm.process_pid);

        break;
      case proc_event::PROC_EVENT_EXIT:
        new_exit(nlcn_msg.proc_ev.event_data.exit.process_pid);

        break;
      default:
        break;
    }
  }
}

std::string Netlink::get_process_name(const int& pid) {
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

std::string Netlink::get_cmdline(const int& pid) {
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

void Netlink::process_event(const int pid) {
  auto name = get_process_name(pid);
  auto cmdline = get_cmdline(pid);

  if (name != "" && cmdline != "") {
    new_exec(pid, name, cmdline);
  }
}
