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

#include "sdb.h"

#define NR_WP 32

typedef struct watchpoint {
  int NO;
  char e[128];       // expression
  word_t old_value;  // the result of expr in the last exec
  struct watchpoint* next;
} WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}

bool new_wp(char* e) {
  if (free_ == NULL) {
    return false;
  }
  WP* free_wp = free_;
  free_ = free_wp->next;
  free_wp->next = head;
  head = free_wp;
  strcpy(free_wp->e, e);
  return true;
}

void free_wp(WP* wp) {
  WP* prev = head;
  while (prev != NULL || prev->next != wp) {
    prev = prev->next;
  }
  // wp is not the head of the used wps
  if (prev != NULL) {
    prev->next = wp->next;
  }
  wp->next = free_;
  free_ = wp;
  // initialize wp
  wp->old_value = 0;
  memset(wp->e, 0, sizeof(wp->e));
}

void watchpoint_display() {
  printf("Num\tWhat\n");
  while (head != NULL) {
    printf("%d\t%s\n", head->NO, head->e);
    head = head->next;
  }
}