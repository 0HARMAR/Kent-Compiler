#include <ctype.h>
#include <stdint.h>
#include <string.h>
#include "symbol_resolve.h"
#include "variable.h"
#include "convert.h"
#include "debug.h"

// 函数声明（确保编译器可见性）
int expr(char **p);
int term(char **p);
int factor(char **p);

// 表达式结果（字符）
char ret_num_str[32];

// 表达式解析入口 
char* expr_proc(char expr_str[30]) {
    char *p = expr_str;
    int result = expr(&p);
    DEBUG_LOG("processed expression : %d",result);
    return int_to_str(result);
}
 
// 表达式层（加减运算）
int expr(char **p) {
    int result = term(p);
    while (1) {
        while (isspace(**p)) (*p)++;
        if (**p != '+' && **p != '-') break;
        char op = **p;
        (*p)++;
        int val = term(p);
        result = (op == '+') ? result + val : result - val;
    }
    return result;
}
 
// 项层（乘除运算）
int term(char **p) {
    int result = factor(p);
    while (1) {
        while (isspace(**p)) (*p)++;
        if (**p != '*' && **p != '/') break;
        char op = **p;
        (*p)++;
        int val = factor(p);
        result = (op == '*') ? result * val : result / val;
    }
    return result;
}
 
// 因子层（数值/变量解析）
int factor(char **p) {
    while (isspace(**p)) (*p)++;
    int num = 0;
    
    // 处理数字字面量（如123）
    if (isdigit(**p)) {
        while (isdigit(**p)) {
            num = num * 10 + (**p - '0');
            (*p)++;
        }
    }
    // 处理变量（如a、var1）
    else if (isalpha(**p)) {
        char var_name[32] = {0};
        int i = 0;
        // 提取变量名（支持字母+数字组合）
        while (isalnum(**p) && i < 31) {
            var_name[i++] = **p;
            (*p)++;
        }
        num = symbol_resolve(var_name); // 调用符号解析函数 
    }
    // 跳过剩余空格 
    while (isspace(**p)) (*p)++;
    return num;
}