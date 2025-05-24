#include <cstdio>

void gen_print_asm(FILE* out){
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
}