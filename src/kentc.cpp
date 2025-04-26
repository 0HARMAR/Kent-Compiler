#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include <string>
#include <iostream>
#include "expr_proc.h"
#include "variable.h"
#include "convert.h"
#include "config.h"
#include "debug.h"

#define LINE_BUF_SIZE 256
#define CODE_BUF_SIZE 4096
Variable vars[MAX_VARS];
int var_count = 0;
uint64_t current_offset = 0;  // 从基地址开始的偏移
FILE *out;

using namespace std;

void walk_through_file_stream(FILE *in);
void process_looper(const char* code); // 处理循环器
int peek_char(FILE *in);

class LineGenerator {
public:
    LineGenerator(const char* input) : input_(input), pos_(0) {
        if (!input_) {
            len_ = 0;
        } else {
            len_ = strlen(input_);
        }
    }

    // Get the next line, returns empty string when done
    std::string next() {
        if (pos_ >= len_) {
            return "";
        }

        std::string line;
        while (pos_ < len_) {
            char c = input_[pos_++];

            // Handle Windows-style \r\n
            if (c == '\r' && pos_ < len_ && input_[pos_] == '\n') {
                pos_++; // Skip \n
                break;
            }
            // Handle Unix-style \n
            if (c == '\n') {
                break;
            }

            line += c;
        }

        return line;
    }

    // Check if there are more lines
    bool hasNext() const {
        return pos_ < len_;
    }

private:
    const char* input_;
    size_t pos_;
    size_t len_;
};

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

void holdPrintln(char var[32]){
    uint64_t o = find_var(var);
    gen_print(out, o, true);
}

void hold_print(char var[32]){
    uint64_t o = find_var(var);
    gen_print(out, o, false);
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
    walk_through_file_stream(in);

    footer(out);
}

void hold_find(char target_var[32]) {
    uint64_t offset = find_var(target_var);
    fprintf(out, "\tlea %lu(%%r15), %%rdi\n", offset);
    fprintf(out, "\tcall print_int\n");
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
        holdPrintln(var1);
    } else if (sscanf(line, "print %s", var1)) {
        hold_print(var1);
    } else if (sscanf(line, "find %s", target_var) == 1) {
        hold_find(target_var);
    }
}

void walk_through_file_stream(FILE *in) {
    // 行处理相关变量
    char line_buf[LINE_BUF_SIZE] = {0};
    int line_pos = 0;

    // looper检测相关变量
    bool in_looper = false;      // 是否处于looper代码块
    char key_buf[16] = {0};      // 关键字缓冲区
    int key_pos = 0;             // 关键字位置

    // 大括号处理相关变量
    int brace_depth = 0;         // 大括号嵌套深度
    char code_buf[CODE_BUF_SIZE] = {0};  // 代码块缓冲区
    int code_pos = 0;            // 代码块位置

    int c;
    while ((c = fgetc(in)) != EOF) {
        if (in_looper) {
            /* 大括号代码块处理模式 */
            // 记录代码字符（保留换行符）
            if (code_pos < CODE_BUF_SIZE-1) {
                code_buf[code_pos++] = c;
            } else {
                fprintf(stderr, "Looper code too long!\n");
                exit(1);
            }

            // 大括号深度计算
            if (c == '{') brace_depth++;
            else if (c == '}') {
                if (--brace_depth == 0) {  // 代码块结束
                    code_buf[code_pos] = '\0';

                    process_looper(code_buf);
                    // 重置状态
                    in_looper = false;
                    code_pos = brace_depth = 0;
                }
            }
        } else {
            /* 正常行处理模式 */
            // 换行符处理逻辑
            if (c == '\r' || c == '\n') {
                // 处理Windows换行
                if (c == '\r' && peek_char(in) == '\n') {
                    fgetc(in); // 消耗\n
                }

                // 处理完整行
                if (line_pos > 0) {
                    line_buf[line_pos] = '\0';
                    process_line(line_buf);
                    line_pos = 0;
                }
                continue;
            }


            // 构建行缓冲区
            if (line_pos < LINE_BUF_SIZE-1) {
                line_buf[line_pos++] = c;
            } else {
                fprintf(stderr, "Line too long!\n");
                exit(1);
            }

            /* 并行检测looper关键字 */
            if (isalpha(c)) {
                // 构建关键字缓冲区（小写）
                if (key_pos < sizeof(key_buf)-1) {
                    key_buf[key_pos++] = tolower(c);
                }
            } else {
                // 非字母字符触发关键字检查
                if (strcmp(key_buf, "looper") == 0) {
                    // 进入looper处理模式
                    in_looper = true;
                    brace_depth = 0;
                    code_pos = 0;

                    // 收集触发后的第一个字符（可能是{）
                    if (code_pos < CODE_BUF_SIZE-1) {
                        code_buf[code_pos++] = c;
                    }

                    // 如果当前字符是大括号则计算深度
                    if (c == '{') brace_depth++;
                }
                // 重置关键字检测
                memset(key_buf, 0, sizeof(key_buf));
                key_pos = 0;
            }
        }
    }

    // 处理文件末尾的未完成行
    if (line_pos > 0) {
        line_buf[line_pos] = '\0';
        process_line(line_buf);
    }

}

// 查看下一个字符的辅助函数
int peek_char(FILE *in) {
    int c = fgetc(in);
    ungetc(c, in);
    return c;
}

void process_looper(const char * code) {
    cout << code << flush;
//    LineGenerator lineGenerator = LineGenerator(code);
//    if(lineGenerator.hasNext()){
//        cout << lineGenerator.next();
//    }
    DEBUG_LOG("looper detected");
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

    out = fopen("output.s", "w");
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