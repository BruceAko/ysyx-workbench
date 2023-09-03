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

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// this should be enough
static char buf[65536] = {};
static char code_buf[65536 + 128] = {};  // a little larger than `buf`
static char* code_format =
    "#include <stdio.h>\n"
    "int main() { "
    "  unsigned result = %s; "
    "  printf(\"%%u\", result); "
    "  return 0; "
    "}";

static int op_count = 0;

static void gen_rand_expr() {
  if (op_count >= 20) {
    int num = rand() % 100;
    char num_s[20];
    sprintf(num_s, "%d", num);
    strcat(buf, num_s);
    return;
  }
  switch (rand() % 3) {
    case 0:
      int num = rand() % 100;
      char num_s[20];
      sprintf(num_s, "%d", num);
      strcat(buf, num_s);
      op_count++;
      break;
    case 1:
      strcat(buf, "(");
      gen_rand_expr();
      strcat(buf, ")");
      op_count++;
      break;
    default:
      gen_rand_expr();
      char op[2];
      op[1] = '\0';
      switch (rand() % 4) {
        case 0:
          op[0] = '+';
          break;
        case 1:
          op[0] = '-';
          break;
        case 2:
          op[0] = '*';
          break;
        default:
          op[0] = '/';
          break;
      }
      strcat(buf, op);
      gen_rand_expr();
      op_count++;
      break;
  }
}

int main(int argc, char* argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i++) {
    buf[0] = '\0';
    op_count = 0;
    gen_rand_expr();

    sprintf(code_buf, code_format, buf);

    FILE* fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr -Wall -Werror");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    ret = fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}
