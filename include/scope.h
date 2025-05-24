//
// Created by hemingyang on 2025/5/5.
//

#ifndef KENT_SCOPE_H
#define KENT_SCOPE_H
typedef enum {
    SCOPE_GLOBAL,     // 全局作用域
    SCOPE_FUNC_LOCAL, // 函数局部作用域
    SCOPE_BLOCK       // 块级作用域（如if/for内部）
} ScopeType;

typedef struct {
    ScopeType type;
    char func_name[32];  // 当type为SCOPE_FUNC_LOCAL时有效
    int block_level;     // 块嵌套层级（从0开始）
} Scope;
#endif //KENT_SCOPE_H
