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

#include <common.h>

#include "./monitor/sdb/sdb.h"

void init_monitor(int, char*[]);
void am_init_monitor();
void engine_start();
int is_exit_status_bad();

int main(int argc, char* argv[]) {
/* Initialize the monitor. */
#ifdef CONFIG_TARGET_AM
  am_init_monitor();
#else
  init_monitor(argc, argv);
#endif

  FILE* fp = fopen("./tools/gen-expr/build/input", "r");
  assert(fp != NULL);
  word_t res;
  char buf[65536];
  while (fscanf(fp, "%d %s\n", &res, buf) != EOF) {
    //fgets(buf, 65536, fp);
    bool success;
    printf("%s\n", buf);
    word_t res2 = expr(buf, &success);
    printf("%u ", res2);
    if (res2 == res && success == true) {
      printf("success\n");
    } else {
      panic("fail\n");
    }
  }

  // bool success;
  // word_t res2 = expr("5*25+17*28", &success);
  // printf("%u ", res2);

  /* Start engine. */
  engine_start();

  return is_exit_status_bad();
}
