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

#ifndef __RISCV_REG_H__
#define __RISCV_REG_H__

#include <common.h>

#define MSTATUS_MIE 0x00000008
#define MSTATUS_MPIE 0x00000080

enum REG_CSR { REG_MTVEC = 0, REG_MCAUSE, REG_MSTATUS, REG_MEPC, REG_SATP, REG_MSCRATCH };

#define CASE(instr_id, enum_id) \
  case instr_id: {              \
    return enum_id;             \
  }

static inline uint8_t csr_id_instr2array(uint32_t instr_id) {
  // Log("the instr_id is %x\n", instr_id);
  switch (instr_id) {
    CASE(0x305, REG_MTVEC);
    CASE(0x342, REG_MCAUSE);
    CASE(0x300, REG_MSTATUS);
    CASE(0x341, REG_MEPC);
    CASE(0x180, REG_SATP);
    CASE(0x340, REG_MSCRATCH);
    default:
      panic("0x%x is NOT a Valid CSR REGISTER!", instr_id);
      return -1;
  }
}

static inline int check_reg_idx(int idx) {
  IFDEF(CONFIG_RT_CHECK, assert(idx >= 0 && idx < MUXDEF(CONFIG_RVE, 16, 32)));
  return idx;
}

#define gpr(idx) (cpu.gpr[check_reg_idx(idx)])
#define csr(idx) (cpu.csr[csr_id_instr2array(idx)])

#define ECALL()                                        \
  {                                                    \
    bool success = false;                              \
    word_t trap_no = isa_reg_str2val("$a7", &success); \
    if (!success) Assert(0, "Invalid gpr register!");  \
    word_t trap_vec = isa_raise_intr(trap_no, s->pc);  \
    s->dnpc = trap_vec;                                \
  }

static inline const char* reg_name(int idx) {
  extern const char* regs[];
  return regs[check_reg_idx(idx)];
}

#endif
