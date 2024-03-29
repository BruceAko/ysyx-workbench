#include <am.h>
#include <nemu.h>

#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T* kbd) {
  kbd->keydown = inl(KBD_ADDR) & KEYDOWN_MASK;   //最高1位
  kbd->keycode = inl(KBD_ADDR) & ~KEYDOWN_MASK;  //低15位
}
