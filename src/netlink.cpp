#include "netlink.hpp"
#include <linux/cn_proc.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fstream>

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

    std::cout << log_tag + "socket creation failed!" << std::endl;
  } else {
    std::cout << log_tag + "socket created" << std::endl;

    sockaddr_nl addr{};

    addr.nl_family = AF_NETLINK;
    addr.nl_groups = CN_IDX_PROC;
    addr.nl_pid = getpid();

    auto rc = bind(nl_socket, (sockaddr*)&addr, sizeof(addr));

    if (rc == -1) {
      listen = false;

      std::cout << log_tag + "socket binding failed!" << std::endl;
    } else {
      std::cout << log_tag + "socket binding succesful!" << std::endl;
    }
  }
}

void Netlink::subscribe() {
  char buff[NLMSG_LENGTH(0)];
  cn_msg cnmsg;
  proc_cn_mcast_op mcast_op;
  iovec iov[3];

  auto* nl_hdr = reinterpret_cast<nlmsghdr*>(buff);

  /* fill the netlink header */

  nl_hdr->nlmsg_len = NLMSG_LENGTH(sizeof(cn_msg) + sizeof(mcast_op));
  nl_hdr->nlmsg_type = NLMSG_DONE;
  nl_hdr->nlmsg_flags = 0;
  nl_hdr->nlmsg_seq = 0;
  nl_hdr->nlmsg_pid = getpid();

  iov[0].iov_base = buff;
  iov[0].iov_len = NLMSG_LENGTH(0);

  /* fill the connector header */

  cnmsg.id.idx = CN_IDX_PROC;
  cnmsg.id.val = CN_VAL_PROC;
  cnmsg.seq = 0;
  cnmsg.ack = 0;
  cnmsg.len = sizeof(mcast_op);

  iov[1].iov_base = &cnmsg;
  iov[1].iov_len = sizeof(cnmsg);

  mcast_op = PROC_CN_MCAST_LISTEN;

  iov[2].iov_base = &mcast_op;
  iov[2].iov_len = sizeof(mcast_op);

  if (writev(nl_socket, iov, 3) == -1) {
    std::cout << log_tag + "failed to send PROC_CN_MCAST_LISTEN!" << std::endl;
  } else {
    std::cout << log_tag + "sent PROC_CN_MCAST_LISTEN to kernel" << std::endl;
  }
}

void Netlink::handle_events() {
  char buff[getpagesize()];
  sockaddr_nl addr{};
  iovec iov[1];

  iov[0].iov_base = buff;
  iov[0].iov_len = sizeof(buff);

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

    auto* nlmsg_hdr = reinterpret_cast<nlmsghdr*>(buff);

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

  auto* event = reinterpret_cast<proc_event*>(msg->data);

  int pid = 0;
  std::string comm;
  std::string cmdline;
  std::string exe_path;
  std::string child_comm;

  switch (event->what) {
    case PROC_EVENT_FORK:
      child_comm = get_comm(event->event_data.fork.child_pid);

      if (!child_comm.empty()) {
        new_fork(event->event_data.fork.child_tgid, event->event_data.fork.child_pid, child_comm);
      }

      break;
    case PROC_EVENT_EXEC:
      pid = event->event_data.exec.process_pid;

      comm = get_comm(pid);
      cmdline = get_cmdline(pid);
      exe_path = get_exe_path(pid);

      if (!comm.empty() && !cmdline.empty()) {
        new_exec(pid, comm, cmdline, exe_path);
      }

      break;
    case PROC_EVENT_COMM:
      pid = event->event_data.comm.process_pid;

      comm = get_comm(pid);
      cmdline = get_cmdline(pid);
      exe_path = get_exe_path(pid);

      if (!comm.empty() && !cmdline.empty()) {
        new_exec(pid, comm, cmdline, exe_path);
      }

      break;
    case PROC_EVENT_EXIT:
      new_exit(event->event_data.exit.process_pid);

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

    return std::string(buff);
  }

  return "";
}