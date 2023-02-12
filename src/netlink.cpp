#include "netlink.hpp"
#include <linux/cn_proc.h>
#include <sys/socket.h>
#include <unistd.h>
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

    struct sockaddr_nl sa_nl {};

    sa_nl.nl_family = AF_NETLINK;
    sa_nl.nl_groups = CN_IDX_PROC;
    sa_nl.nl_pid = getpid();

    int rc = -1;

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
      enum proc_cn_mcast_op cn_mcast;
      struct cn_msg cn_msg;
    };
  } nlcn_msg{};

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
      struct proc_event proc_ev;
      struct cn_msg cn_msg;
    };
  } nlcn_msg{};

  while (listen) {
    recv(nl_socket, &nlcn_msg, sizeof(nlcn_msg), 0);

    int pid = 0;
    std::string comm;
    std::string cmdline;
    std::string exe_path;
    std::string child_comm;

    switch (nlcn_msg.proc_ev.what) {
      case proc_event::PROC_EVENT_FORK:
        child_comm = get_comm(nlcn_msg.proc_ev.event_data.fork.child_pid);

        if (!child_comm.empty()) {
          new_fork(nlcn_msg.proc_ev.event_data.fork.child_tgid, nlcn_msg.proc_ev.event_data.fork.child_pid, child_comm);
        }

        break;
      case proc_event::PROC_EVENT_EXEC:
        pid = nlcn_msg.proc_ev.event_data.exec.process_pid;

        comm = get_comm(pid);
        cmdline = get_cmdline(pid);
        exe_path = get_exe_path(pid);

        if (!comm.empty() && !cmdline.empty()) {
          new_exec(pid, comm, cmdline, exe_path);
        }

        break;
      case proc_event::PROC_EVENT_COMM:
        pid = nlcn_msg.proc_ev.event_data.comm.process_pid;

        comm = get_comm(pid);
        cmdline = get_cmdline(pid);
        exe_path = get_exe_path(pid);

        if (!comm.empty() && !cmdline.empty()) {
          new_exec(pid, comm, cmdline, exe_path);
        }

        break;
      case proc_event::PROC_EVENT_EXIT:
        new_exit(nlcn_msg.proc_ev.event_data.exit.process_pid);

        break;
      default:
        break;
    }
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

    return std::string(buff);
  }

  return "";
}