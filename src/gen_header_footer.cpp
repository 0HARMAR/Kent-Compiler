//
// Created by hemingyang on 2025/4/30.
//

#include <cstdio>
#include "config.h"

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