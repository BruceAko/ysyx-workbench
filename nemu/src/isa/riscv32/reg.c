/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include "local-include/reg.h"

#include <isa.h>

const char* regs[] = {"$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2", "s0", "s1", "a0",  "a1",  "a2", "a3", "a4", "a5",
                      "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7", "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"};

const char* csrs[] = {"mtvec", "mcause", "mstatus", "mepc", "satp", "mscratch"};

void isa_reg_display() {
  printf("%s\t0x%x\t%u\n", "pc", cpu.pc, cpu.pc);
  for (int i = 0; i < 32; i++) {
    printf("%s\t0x%x\t%u\n", regs[i], gpr(i), gpr(i));
  }
}

word_t isa_reg_str2val(const char* s, bool* success) {
  *success = false;
  if (strcmp("pc", s + 1) == 0) {
    *success = true;
    return cpu.pc;
  }
  for (int i = 0; i < MUXDEF(CONFIG_RVE, 16, 32); ++i) {
    if (strcmp(regs[i], s + 1) == 0) {
      *success = true;
      return gpr(i);
    }
  }
  return 0;
}
