//
// Created by hemingyang on 2025/4/30.
//

#include "extract_operation_block.h"

#include <cstdio>
#include <string>
#include <cctype>

// 声明处理函数
template<typename T>
void process_operation_block(T block);

// 块特性模板（需特化实现）
template<typename Block>
struct BlockTraits;

// 字符串块特性特化
template<>
struct BlockTraits<std::string> {
    using Position = size_t;

    static void init(std::string& block, size_t& pos) { pos = 0; }

    static char current(std::string& block, size_t pos) {
        return pos < block.size() ? block[pos] : EOF;
    }

    static void advance(std::string&, size_t& pos) { ++pos; }

    static void skip_whitespace(std::string& block, size_t& pos) {
        while (pos < block.size() && isspace(block[pos])) ++pos;
    }

    static std::string extract(std::string& block, size_t start, size_t end) {
        return block.substr(start, end - start + 1);
    }
};

// 文件块特性特化
template<>
struct BlockTraits<FILE*> {
    using Position = long;

    static void init(FILE* file, long& pos) {
        pos = ftell(file);
    }

    static char current(FILE* file, long pos) {
        const long current = ftell(file);
        fseek(file, pos, SEEK_SET);
        const int ch = fgetc(file);
        fseek(file, current, SEEK_SET);
        return ch;
    }

    static void advance(FILE* file, long& pos) {
        fseek(file, pos, SEEK_SET);
        fgetc(file);
        pos = ftell(file);
    }

    static void skip_whitespace(FILE* file, long& pos) {
        fseek(file, pos, SEEK_SET);
        int ch;
        while ((ch = fgetc(file)) != EOF && isspace(ch)) ++pos;
        fseek(file, pos, SEEK_SET);
    }

    static std::string extract(FILE* file, long start, long end) {
        std::string result;
        const long current = ftell(file);
        fseek(file, start, SEEK_SET);

        for (long i = start; i <= end; ++i) {
            const int ch = fgetc(file);
            if (ch == EOF) break;
            result += static_cast<char>(ch);
        }

        fseek(file, current, SEEK_SET);
        return result;
    }
};

template <typename Block>
void extract_operation_block(Block block) {
    using Traits = BlockTraits<Block>;
    using Position = typename Traits::Position;

    Position start_pos, current_pos;
    Traits::init(block, current_pos);

    // 跳过空白字符
    Traits::skip_whitespace(block, current_pos);

    // 处理参数括号
    if (Traits::current(block, current_pos) == '(') {
        int paren_count = 1;
        Traits::advance(block, current_pos);

        while (paren_count > 0) {
            const char ch = Traits::current(block, current_pos);
            if (ch == '(') ++paren_count;
            else if (ch == ')') --paren_count;
            if (ch == EOF) return; // 错误处理
            Traits::advance(block, current_pos);
        }
    }

    // 定位代码块起始位置
    Traits::skip_whitespace(block, current_pos);
    if (Traits::current(block, current_pos) != '{') return;

    start_pos = current_pos;
    int brace_count = 1;
    Traits::advance(block, current_pos);

    // 匹配嵌套大括号
    while (brace_count > 0) {
        const char ch = Traits::current(block, current_pos);
        if (ch == '{') ++brace_count;
        else if (ch == '}') --brace_count;
        if (ch == EOF) return; // 错误处理

        // 处理字符串字面量（简单跳过）
        if (ch == '"') {
            Traits::advance(block, current_pos);
            while (Traits::current(block, current_pos) != '"' &&
                   Traits::current(block, current_pos) != EOF) {
                Traits::advance(block, current_pos);
            }
        }

        Traits::advance(block, current_pos);
    }

    // 提取并处理代码块
    const auto code_block = Traits::extract(block, start_pos, current_pos - 1);
    process_operation_block(code_block);
}

// 示例处理函数实现
template<>
void process_operation_block<std::string>(std::string block) {
    printf("Processing block:\n%s\n", block.c_str());
}
