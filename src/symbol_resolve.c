#include "variable.h"
#include <stdlib.h>
#include <stdio.h>
#include "config.h"

extern Variable vars[MAX_VARS];

int symbol_resolve(char name[32]) {
    // 遍历符号表查找匹配项
    for (int i = 0; i < MAX_VARS; i++) {
        // 比较变量名（考虑字符串终止符）
        if (strncmp(vars[i].name, name, sizeof(vars[i].name)) == 0) {
            // 将字符串类型的value转换为整型返回
            return atoi(vars[i].value);
        }
    }
    
    // 未找到处理逻辑
    fprintf(stderr, "Error: Undefined variable '%s'\n", name);
    exit(EXIT_FAILURE);  // 立即终止程序
    return -1;  // 保持编译器静默
}