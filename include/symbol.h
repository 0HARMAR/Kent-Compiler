// symbol.h
#ifndef SYMBOL_H
#define SYMBOL_H

#include <stdint.h>
#include "scope.h"  // 包含之前定义的作用域元数据
#include "scope_stack.h"
#include "type.h"

// 符号类型枚举
typedef enum {
    SYM_VARIABLE,     // 变量
    SYM_FUNCTION,     // 函数
    SYM_DATASECTION,  // 数据区（如.data/.bss）
    SYM_CONSTANT,     // 常量
    SYM_STRUCT,       // 结构体类型
    SYM_ENUM,         // 枚举类型
    SYM_TYPEDEF,      // 类型定义
    SYM_LABEL         // 代码标签
} SymbolType;

// 符号属性结构体
typedef struct {
    char name[64];              // 符号名称
    SymbolType sym_type;        // 符号类型
    DataType data_type;         // 数据类型
    Scope scope;            // 作用域信息
    uint64_t mem_address;       // 内存地址/偏移量
    int size;                   // 占用空间大小（字节）

    union {
        // 函数特有属性
        struct {
            DataType ret_type;  // 返回类型
            int param_count;    // 参数个数
            // 可扩展参数类型列表
        } func_attr;

        // 数据区特有属性
        struct {
            char section_name[32];  // 所属数据段名（如".data"）
            int alignment;          // 内存对齐要求
        } section_attr;

        // 类型定义特有
        struct {
            int type_id;        // 自定义类型ID
            char type_name[32]; // 类型名称
        } type_attr;
    };
} Symbol;

// 符号表操作接口
typedef struct {
    Symbol* symbols;         // 符号数组
    int capacity;           // 表容量
    int count;              // 当前符号数量
    ScopeMetaStack* scopes; // 作用域堆栈
} SymbolTable;

// 符号表操作函数
void symtab_init(SymbolTable* tab, int initial_cap);
void symtab_add(SymbolTable* tab, const Symbol* sym);
const Symbol* symtab_find(const SymbolTable* tab, const char* name);
void symtab_enter_scope(SymbolTable* tab, const Scope * new_scope);
void symtab_exit_scope(SymbolTable* tab);

#endif // SYMBOL_H