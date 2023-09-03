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

#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

enum {
  TK_NOTYPE = 256,
  TK_EQ,
  TK_NUM,
};

static struct rule {
  const char* regex;
  int token_type;
} rules[] = {
    {" +", TK_NOTYPE},   // spaces
    {"\\+", '+'},        // plus
    {"-", '-'},          // subtract
    {"\\*", '*'},        // multiply
    {"/", '/'},          // divide
    {"==", TK_EQ},       // equal
    {"[0-9]+", TK_NUM},  // num
    {"\\(", '('},        // open parenthesis
    {"\\)", ')'},        // close parenthesis
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[128] __attribute__((used)) = {};
static int nr_token __attribute__((used)) = 0;

static bool make_token(char* e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 &&
          pmatch.rm_so == 0) {
        char* substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i,
            rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        switch (rules[i].token_type) {
          case TK_NOTYPE:
            break;
          case TK_NUM:
            assert(substr_len < 32);
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            tokens[nr_token].str[substr_len] = '\0';
          // write through
          default:
            tokens[nr_token].type = rules[i].token_type;
            nr_token++;
            break;
        }
        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

bool check_parentheses(int p, int q, bool* legal) {
  bool result = true;
  int s = 0;
  for (int i = p; i <= q; i++) {
    if (tokens[i].type == '(') s++;
    if (tokens[i].type == ')') s--;
    if (s == 0 && i != q) result = false;
  }
  if (s != 0) {
    *legal = false;
    return true;
  }
  *legal = true;
  return result;
}

int find_main_op(int p, int q) {
  int position = -1;
  int left_parentheses = 0;
  for (int i = p; i <= q; i++) {
    switch (tokens[i].type) {
      case '(':
        left_parentheses++;
        break;
      case ')':
        left_parentheses--;
        break;
      case '*':
      case '/':
        if (left_parentheses > 0) continue;
        if (position != -1) {
          int type = tokens[position].type;
          if (type == '*' || type == '/') position = i;
        } else {
          position = i;
        }
        break;
      case '+':
      case '-':
        if (left_parentheses > 0) continue;
        position = i;
        break;
      default:
        continue;
    }
  }
  assert(position >= p && position <= q);
  return position;
}

word_t eval(int p, int q) {
  bool legal;
  if (p > q) {
    panic("Bad expression");
  } else if (p == q) {
    return atoi(tokens[p].str);
  } else if (check_parentheses(p, q, &legal) == true) {
    if (legal == false) {
      panic("Unmatched parentheses");
    }
    return eval(p + 1, q - 1);
  } else {
    int op_pos = find_main_op(p, q);
    int val1 = eval(p, op_pos - 1);
    int val2 = eval(op_pos + 1, q);
    switch (tokens[op_pos].type) {
      case '+':
        return val1 + val2;
      case '-':
        return val1 - val2;
      case '*':
        return val1 * val2;
      case '/':
        if (val2 == 0) panic("Division by zero");
        return val1 / val2;
      default:
        panic("Wrong operator");
    }
  }
}

word_t expr(char* e, bool* success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  *success = true;
  return eval(0, nr_token - 1);
}
