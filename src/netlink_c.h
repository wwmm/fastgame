#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <bits/types/struct_iovec.h>
#include <linux/connector.h>

/*
    Copied from <linux/cn_proc.h>
*/

enum fg_proc_cn_event {
  FG_PROC_EVENT_NONE = 0x00000000,
  FG_PROC_EVENT_FORK = 0x00000001,
  FG_PROC_EVENT_EXEC = 0x00000002,
  FG_PROC_EVENT_UID = 0x00000004,
  FG_PROC_EVENT_GID = 0x00000040,
  FG_PROC_EVENT_SID = 0x00000080,
  FG_PROC_EVENT_PTRACE = 0x00000100,
  FG_PROC_EVENT_COMM = 0x00000200,
  FG_PROC_EVENT_NONZERO_EXIT = 0x20000000,
  FG_PROC_EVENT_COREDUMP = 0x40000000,
  FG_PROC_EVENT_EXIT = 0x80000000
};

struct fg_cn_msg {
  int process_pid;
  int child_pid;
  int child_tgid;

  enum fg_proc_cn_event event;
};

void prepare_iovec(struct iovec* iov);

struct fg_cn_msg parse_cn_msg(struct cn_msg* msg);

#ifdef __cplusplus
}
#endif