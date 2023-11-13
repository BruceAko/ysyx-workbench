#include <common.h>

void exit(int status);

enum {
  SYS_exit = 0,
  SYS_yield,
  SYS_open,
  SYS_read,
  SYS_write,
  SYS_kill,
  SYS_getpid,
  SYS_close,
  SYS_lseek,
  SYS_brk,
  SYS_fstat,
  SYS_time,
  SYS_signal,
  SYS_execve,
  SYS_fork,
  SYS_link,
  SYS_unlink,
  SYS_wait,
  SYS_times,
  SYS_gettimeofday
};

void do_syscall(Context* c) {
  uintptr_t a[4];
  a[0] = c->GPR1;

  switch (a[0]) {
    case SYS_exit:
      exit((int)c->GPR2);
      break;
    case SYS_yield:
      yield();
      c->GPRx = 0;
      break;
    default:
      panic("Unhandled syscall ID = %d", a[0]);
  }
}
