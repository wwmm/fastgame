#include "netlink_c.h"
#include <bits/types/struct_iovec.h>
#include <linux/cn_proc.h>
#include <linux/connector.h>
#include <linux/netlink.h>
#include <unistd.h>

void prepare_iovec(struct iovec* iov) {
  char buff[NLMSG_LENGTH(0)];
  struct cn_msg cnmsg;
  enum proc_cn_mcast_op mcast_op = PROC_CN_MCAST_LISTEN;

  struct nlmsghdr* nl_hdr = (struct nlmsghdr*)buff;

  /* fill the netlink header */

  nl_hdr->nlmsg_len = NLMSG_LENGTH(sizeof(struct cn_msg) + sizeof(mcast_op));
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
}

struct fg_cn_msg parse_cn_msg(struct cn_msg* msg) {
  struct proc_event* event = (struct proc_event*)msg->data;

  struct fg_cn_msg out = {.process_pid = -1, .child_pid = -1, .child_tgid = -1, .event = FG_PROC_EVENT_NONE};

  switch (event->what) {
    case PROC_EVENT_FORK:
      out.event = FG_PROC_EVENT_FORK;

      out.child_pid = event->event_data.fork.child_pid;
      out.child_tgid = event->event_data.fork.child_tgid;

      break;
    case PROC_EVENT_EXEC:
      out.event = FG_PROC_EVENT_EXEC;

      out.process_pid = event->event_data.exec.process_pid;

      break;
    case PROC_EVENT_COMM:
      break;
      out.event = FG_PROC_EVENT_COMM;

      out.process_pid = event->event_data.exec.process_pid;

      break;
    case PROC_EVENT_NONZERO_EXIT:
      out.event = FG_PROC_EVENT_EXIT;

      out.process_pid = event->event_data.exec.process_pid;

      break;
    default:
      out.event = FG_PROC_EVENT_NONE;
      break;
  }

  return out;
}