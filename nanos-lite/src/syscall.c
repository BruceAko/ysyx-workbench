#include "syscall.h"

#include <common.h>
#include <fs.h>
#include <proc.h>

typedef struct timeval {
  int32_t tv_sec;  /* seconds */
  int32_t tv_usec; /* microseconds */
} Timeval;
typedef struct timezone {
  int tz_minuteswest; /* minutes west of Greenwich */
  int tz_dsttime;     /* type of DST correction */
} Timezone;

static AM_TIMER_UPTIME_T uptime;

static int gettimeofday(Timeval* tv, Timezone* tz) {
  ioe_read(AM_TIMER_UPTIME, &uptime);
  // important: must convert to int32_t as tv_usec/sec is int32_t
  //            uptime.us is uint64_t, cause overflow!!!!!!!!!!!
  //            make the tv_sec is always 0.....
  tv->tv_usec = (int32_t)uptime.us;
  tv->tv_sec = (int32_t)uptime.us / 1000000;
  // printf("sec is %d, usec is %d\n",tv->tv_sec,tv->tv_usec);
  return 0;
}

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
      c->GPRx = (int)gettimeofday((Timeval*)a[1], (Timezone*)a[2]);
      break;
    default:
      panic("Unhandled syscall ID = %d", a[0]);
  }
}
