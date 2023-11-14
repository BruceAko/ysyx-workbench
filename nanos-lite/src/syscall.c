#include "syscall.h"

#include <common.h>
#include <fs.h>
#include <memory.h>

void exit(int status);

void do_syscall(Context* c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;

  switch (a[0]) {
    case SYS_exit:
      exit((int)a[1]);
      break;
    case SYS_yield:
      yield();
      c->GPRx = 0;
      break;
    case SYS_open:
      c->GPRx = fs_open((const char*)a[1], (int)a[2], (int)a[3]);
      break;
    case SYS_write:
      c->GPRx = fs_write((int)a[1], (const void*)a[2], (size_t)a[3]);
      break;
    case SYS_read:
      c->GPRx = fs_read((int)a[1], (void*)a[2], (size_t)a[3]);
      break;
    case SYS_lseek:
      c->GPRx = fs_lseek((int)a[1], (size_t)a[2], (int)a[3]);
      break;
    case SYS_close:
      c->GPRx = fs_close((int)a[1]);
      break;
    case SYS_execve:
      // c->GPRx = execve((const char*)a[1], (char** const)a[2], (char** const)a[3]);
      break;
    case SYS_brk:
      c->GPRx = (int)mm_brk((uintptr_t)a[1]);
      break;
    case SYS_gettimeofday:
      // c->GPRx = (int)gettimeofday((Timeval*)a[1], (Timezone*)a[2]);
      break;
    default:
      panic("Unhandled syscall ID = %d", a[0]);
  }
}
