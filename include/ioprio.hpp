#ifndef IOPRIO_HPP
#define IOPRIO_HPP

/*
    The code below was taken from ionice sources
*/

#include <sys/syscall.h>
#include <unistd.h>

#define IOPRIO_CLASS_SHIFT (13)
#define IOPRIO_PRIO_VALUE(class, data) (((class) << IOPRIO_CLASS_SHIFT) | data)

enum {
  IOPRIO_CLASS_NONE,
  IOPRIO_CLASS_RT,
  IOPRIO_CLASS_BE,
  IOPRIO_CLASS_IDLE,
};

enum {
  IOPRIO_WHO_PROCESS = 1,
  IOPRIO_WHO_PGRP,
  IOPRIO_WHO_USER,
};

static inline auto ioprio_set_realtime(int pid, int priority) -> long {
  return syscall(SYS_ioprio_set, IOPRIO_WHO_PROCESS, pid, IOPRIO_PRIO_VALUE(IOPRIO_CLASS_RT, priority));
}

#endif