#include <stdlib.h>

extern char ret_num_str[32];

// 最简单实现（假设输入字符串一定是有效整数）
int str_to_int(const char str[32]) {
    return atoi(str);  // 直接调用标准库函数
}

// 将 int 转换为字符串并存入全局变量
char* int_to_str(int num) {
    // 安全格式化（防止缓冲区溢出）
    snprintf(ret_num_str, sizeof(ret_num_str), "%d", num);
    return ret_num_str; // 返回全局变量的地址
}