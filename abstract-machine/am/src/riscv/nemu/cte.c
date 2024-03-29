#include <am.h>
#include <klib.h>
#include <riscv/riscv.h>

static Context* (*user_handler)(Event, Context*) = NULL;

void display_context(Context* c) {
  for (int i = 0; i < sizeof(c->gpr) / sizeof(c->gpr[0]); ++i) printf("x%d is %p\n", i, c->gpr[i]);
  printf("mcause, mstatus, mepc is %p, %p, %p\n", c->mcause, c->mstatus, c->mepc);
}

Context* __am_irq_handle(Context* c) {
  if (user_handler) {
    Event ev = {0};
    switch (c->mcause) {
      case -1:
        ev.event = EVENT_YIELD;
        c->mepc += 4;
        break;
      case 0 ... 19:
        ev.event = EVENT_SYSCALL;
        c->mepc += 4;
        break;
      default:
        ev.event = EVENT_ERROR;
        break;
    }
    //display_context(c);
    c = user_handler(ev, c);
    assert(c != NULL);
  }
  return c;
}

extern void __am_asm_trap(void);

bool cte_init(Context* (*handler)(Event, Context*)) {
  // initialize exception entry
  asm volatile("csrw mtvec, %0" : : "r"(__am_asm_trap));

  // register event handler
  user_handler = handler;

  return true;
}

Context* kcontext(Area kstack, void (*entry)(void*), void* arg) { return NULL; }

void yield() {
#ifdef __riscv_e
  asm volatile("li a5, -1; ecall");
#else
  asm volatile("li a7, -1; ecall");
#endif
}

bool ienabled() { return false; }

void iset(bool enable) {}
