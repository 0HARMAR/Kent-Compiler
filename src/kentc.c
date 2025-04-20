#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "expr_proc.h"
#include "variable.h"
#include "convert.h"
#include "config.h"
#include "debug.h"

Variable vars[MAX_VARS];
int var_count = 0;
uint64_t current_offset = 0;  // 从基地址开始的偏移

// 生成汇编头（包含mmap调用和基地址加载）
void header(FILE *out) {
    fprintf(out, ".section .data\n");
    fprintf(out, "buffer: .fill 20\n");
    fprintf(out, ".section .text\n");
    fprintf(out, ".globl _start\n");
    fprintf(out, ".code64\n");
    fprintf(out, "_start:\n");
    
    // 通过mmap分配固定地址内存（需要MAP_FIXED标志）
    fprintf(out, "\t# 分配内存到0x500000\n");
    fprintf(out, "\tmov $9, %%rax\n");          // mmap系统调用号
    fprintf(out, "\tmov $0x500000, %%rdi\n");   // 目标地址
    fprintf(out, "\tmov $%d, %%rsi\n", PAGE_SIZE); // 分配大小
    fprintf(out, "\tmov $3, %%rdx\n");          // PROT_READ|PROT_WRITE
    fprintf(out, "\tmov $0x1022, %%r10\n");      // MAP_FIXED|MAP_PRIVATE|MAP_ANONYMOUS
    fprintf(out, "\tmov $-1, %%r8\n");          // fd
    fprintf(out, "\txor %%r9, %%r9\n");         // offset
    fprintf(out, "\tsyscall\n");
    
    // 错误检查
    fprintf(out, "\ttest %%rax,%%rax\n");
    fprintf(out, "\tjs .error\n");
    
    // 将基地址加载到R15寄存器
    fprintf(out, "\tmov $0x500000, %%r15\n");
}

// 生成汇编尾
void footer(FILE *out) {
    fprintf(out, "\tjmp .exit\n");
    fprintf(out, ".error:\n");
    fprintf(out, "\tmov $60, %%rax\n");
    fprintf(out, "\tmov $1, %%rdi\n");
    fprintf(out, "\tsyscall\n");
    fprintf(out, ".exit:\n");
    fprintf(out, "\tmov $60, %%rax\n");
    fprintf(out, "\txor %%rdi, %%rdi\n");
    fprintf(out, "\tsyscall\n");
}

// 变量地址分配（返回相对于基地址的偏移）
uint64_t allocate_var(const char *name, int size, uint64_t specific_offset) {
    if (specific_offset != 0) {
        for (int i = 0; i < var_count; i++) {
            if (vars[i].offset == specific_offset) {
                fprintf(stderr, "Error: Offset 0x%lx conflict\n", specific_offset);
                exit(1);
            }
        }
        return specific_offset;
    }
    
    // 检查内存边界
    if (current_offset + size > PAGE_SIZE) {
        fprintf(stderr, "Error: Out of memory\n");
        exit(1);
    }
    
    uint64_t offset = current_offset;
    current_offset += size;
    return offset;
}

// 查找变量偏移量
uint64_t find_var(const char *name) {
    for (int i = 0; i < var_count; i++) {
        if (strcmp(vars[i].name, name) == 0) {
            return vars[i].offset;
        }
    }
    return 0;
}

void gen_store(FILE *out, uint64_t offset, int value) {
    fprintf(out, "\tmovl $%d, %%eax\n", value);    // 使用临时寄存器
    fprintf(out, "\tmovl %%eax, %lu(%%r15)\n", offset);
}

// 生成加法指令（基于寄存器寻址）
void gen_add(FILE *out, uint64_t dest_offset, uint64_t src1_offset, uint64_t src2_offset) {
    fprintf(out, "\tmovl %lu(%%r15), %%eax\n", src1_offset);
    fprintf(out, "\taddl %lu(%%r15), %%eax\n", src2_offset);
    fprintf(out, "\tmovl %%eax, %lu(%%r15)\n", dest_offset);
}

// 生成打印指令
void gen_print(FILE *out, uint64_t offset,bool newline) {
    fprintf(out, "\tmovl %lu(%%r15), %%eax\n", offset);
    fprintf(out, "\tmov %%rax, %%rdi\n");
    if(newline)
    fprintf(out,"\tmov $1, %%rsi\n");
    else
    fprintf(out,"\tmov $0, %%rsi\n");
    fprintf(out, "\tcall print_int\n");
}

// 主编译函数
void compile(FILE *in, FILE *out) {
    char line[256];
    
    // 添加打印函数（适配寄存器寻址）
    fprintf(out, "print_int:\n");
    fprintf(out,"\tmov %%rsi,%%r8\n");
    fprintf(out, "\tlea buffer(%%rip), %%rsi\n");
    fprintf(out, "\tcall itoa\n");
    fprintf(out,"\ttest %%r8,%%r8\n");
    fprintf(out,"\tmov $10, %%rax\n");
    fprintf(out,"\tjz skip_call\n");
    fprintf(out,"\tcall newline\n");
    fprintf(out,"skip_call:\n");
    fprintf(out,"\tmov %%rax,%%r8\n");
    fprintf(out, "\tmov $1, %%rax\n");
    fprintf(out, "\tmov $1, %%rdi\n");
    fprintf(out, "\tlea buffer(%%rip), %%rsi\n");
    fprintf(out, "\tmov %%r8, %%rdx\n");
    fprintf(out, "\tsyscall\n");
    fprintf(out, "\tret\n\n");

    // 插入新行
    fprintf(out, "newline:\n");
    fprintf(out,"\tlea buffer(%%rip), %%r8\n");
    fprintf(out, "\tmovb $0xA, 11(%%r8)\n");
    fprintf(out,"\tmov $12, %%rax\n");
    fprintf(out,"\tret\n");

    // 64位版itoa实现
    fprintf(out, "itoa:\n");
    fprintf(out, "\tmov %%rdi, %%rax\n");      // 参数1：数字
    fprintf(out, "\tmov %%rsi, %%rdi\n");      // 参数2：缓冲区地址
    fprintf(out, "\tmov $10, %%rbx\n");
    fprintf(out, "\tmov $0, %%rcx\n");
    
    fprintf(out, "\tcmp $0, %%rax\n");
    fprintf(out, "\tjge 1f\n");
    fprintf(out, "\tneg %%rax\n");
    fprintf(out, "\tmovb $'-', (%%rdi)\n");
    fprintf(out, "\tinc %%rdi\n");
    fprintf(out, "1:\n");
    
    fprintf(out, "\tcmp $0, %%rax\n");
    fprintf(out, "\tjnz 2f\n");
    fprintf(out, "\tmovb $'0', (%%rdi)\n");
    fprintf(out, "\tinc %%rdi\n");
    fprintf(out, "\tjmp 4f\n");
    
    fprintf(out, "2:\n");
    fprintf(out, "\txor %%rdx, %%rdx\n");
    fprintf(out, "\tdiv %%rbx\n");           // 使用64位除法
    fprintf(out, "\tadd $'0', %%dl\n");
    fprintf(out, "\tpush %%dx\n");
    fprintf(out, "\tinc %%rcx\n");
    fprintf(out, "\ttest %%rax, %%rax\n");
    fprintf(out, "\tjnz 2b\n");
    
    fprintf(out, "3:\n");
    fprintf(out, "\tpop %%dx\n");
    fprintf(out, "\tmovb %%dl, (%%rdi)\n");
    fprintf(out, "\tinc %%rdi\n");
    fprintf(out, "\tloop 3b\n");
    
    fprintf(out, "4:\n");
    fprintf(out, "\tmovb $0, (%%rdi)\n");
    fprintf(out, "\tret\n\n");
    
    
    header(out);

    while (fgets(line, sizeof(line), in)) {
        char cmd[32], var1[32], var2[32], var3[32];
        char target_var[32];
        uint64_t offset;
        char expr[30];
        int value;
        
        char* at_pos = strstr(line, " at ");
        if (at_pos) {
            *at_pos = '\0';  // 临时截断字符串
            sscanf(line, "set int %s = %29[^\n]", var1, expr);
            sscanf(at_pos + 4, "0x%lx", &offset);  // 跳过"at "后的地址

            if (offset >= PAGE_SIZE) {
                fprintf(stderr, "Error: Offset 0x%lx exceeds page\n", offset);
                exit(1);
            }

            DEBUG_LOG("Compiling expression: %s", expr);
            vars[var_count] = (Variable){.offset=offset};
            strncpy(vars[var_count].name, var1, 31);
            strncpy(vars[var_count].value, expr_proc(expr), sizeof(vars[var_count].value) - 1);
            vars[var_count].value[sizeof(vars[var_count].value) - 1] = '\0'; // 强制终止
            int value_ = str_to_int(vars[var_count].value);
            gen_store(out, offset, value_);
            var_count++;
        }
        else if (sscanf(line, "int %s = %d", var1, &value) == 2) {
            uint64_t o = allocate_var(var1, 4, 0);
            vars[var_count] = (Variable){.offset=o};
            strncpy(vars[var_count].name, var1, 31);
            var_count++;
            gen_store(out, o, value);
        }
        else if (sscanf(line, "int %s = %s + %s", var1, var2, var3) == 3) {
            uint64_t dest = allocate_var(var1, 4, 0);
            uint64_t src1 = find_var(var2);
            uint64_t src2 = find_var(var3);
            vars[var_count] = (Variable){.offset=dest};
            strncpy(vars[var_count].name, var1, 31);
            var_count++;
            gen_add(out, dest, src1, src2);
        }
        else if (sscanf(line, "println %s", var1)){
            DEBUG_LOG("CALLED");
            uint64_t o = find_var(var1);
            gen_print(out, o,true);
        }
        else if (sscanf(line, "print %s", var1)){
            uint64_t o = find_var(var1);
            gen_print(out, o,false);
        }
        else if (strstr(line, "looper") == line) {
            DEBUG_LOG("looper deteched");
        }
        else if (sscanf(line, "find %s", target_var) == 1) {
            uint64_t offset = find_var(target_var);
            fprintf(out,"\tlea %lu(%%r15),%%rdi\n",offset);
            fprintf(out, "\tcall print_int\n");
    }
    }

    footer(out);
}

void process_line(char *line){
    char var1[32], var2[32], var3[32], target_var[32];
    uint64_t offset;
    char expr[30];
    int value;

    // 去除可能的换行符（已处理，但确保安全）
    line[strcspn(line, "\r\n")] = '\0';

    char* at_pos = strstr(line, " at ");
    if (at_pos) {
        *at_pos = '\0';
        sscanf(line, "set int %s = %29[^\n]", var1, expr);
        sscanf(at_pos + 4, "0x%lx", &offset);

        if (offset >= PAGE_SIZE) {
            fprintf(stderr, "Error: Offset 0x%lx exceeds page\n", offset);
            exit(1);
        }

        // 假设expr_proc, gen_store等函数存在
        DEBUG_LOG("Compiling expression: %s", expr);
        strncpy(vars[var_count].name, var1, 31);
        vars[var_count].offset = offset;
        strncpy(vars[var_count].value, expr_proc(expr), 31);
        vars[var_count].value[31] = '\0';
        int value_ = str_to_int(vars[var_count].value);
        gen_store(out, offset, value_);
        var_count++;
    } else if (sscanf(line, "int %s = %d", var1, &value) == 2) {
        uint64_t o = allocate_var(var1, 4, 0); // 假设allocate_var存在
        vars[var_count].offset = o;
        strncpy(vars[var_count].name, var1, 31);
        var_count++;
        gen_store(out, o, value);
    } else if (sscanf(line, "int %s = %s + %s", var1, var2, var3) == 3) {
        uint64_t dest = allocate_var(var1, 4, 0);
        uint64_t src1 = find_var(var2); // 假设find_var存在
        uint64_t src2 = find_var(var3);
        vars[var_count].offset = dest;
        strncpy(vars[var_count].name, var1, 31);
        var_count++;
        gen_add(out, dest, src1, src2);
    } else if (sscanf(line, "println %s", var1)) {
        uint64_t o = find_var(var1);
        gen_print(out, o, true);
    } else if (sscanf(line, "print %s", var1)) {
        uint64_t o = find_var(var1);
        gen_print(out, o, false);
    } else if (strstr(line, "looper") == line) {
        DEBUG_LOG("looper detected");
    } else if (sscanf(line, "find %s", target_var) == 1) {
        uint64_t offset = find_var(target_var);
        fprintf(out, "\tlea %lu(%%r15), %%rdi\n", offset);
        fprintf(out, "\tcall print_int\n");
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input>\n", argv[0]);
        return 1;
    }

    FILE *in = fopen(argv[1], "r");
    if (!in) {
        perror("Error opening input");
        return 1;
    }

    FILE *out = fopen("output.s", "w");
    if (!out) {
        perror("Error opening output");
        fclose(in);
        return 1;
    }

    compile(in, out);

    fclose(in);
    fclose(out);
    
    // 64位汇编命令
    system("as --64 -g -o output.o output.s");
    system("ld -m elf_x86_64 -o a.out output.o");
    
    return 0;
}