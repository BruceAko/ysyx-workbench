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

#include <../local-include/reg.h>
#include <isa.h>

word_t isa_raise_intr(word_t NO, vaddr_t epc) {
  cpu.csr[REG_MEPC] = epc;
  cpu.csr[REG_MCAUSE] = NO;

  if (cpu.csr[REG_MSTATUS] & MSTATUS_MIE)
    cpu.csr[REG_MSTATUS] |= MSTATUS_MPIE;
  else
    cpu.csr[REG_MSTATUS] &= (~MSTATUS_MPIE);
  cpu.csr[REG_MSTATUS] &= (~MSTATUS_MIE);

  return cpu.csr[REG_MTVEC];
}

word_t isa_query_intr() { return INTR_EMPTY; }
