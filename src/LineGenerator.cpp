#include <cstdio>
#include "LineGenerator.h"

// 构造函数的实现
LineGenerator::LineGenerator(const char* input) : input_type_(STRING) {
    str_input_.input = input;
    str_input_.pos = 0;
    str_input_.len = (input != nullptr) ? strlen(input) : 0;
    line_starts_.push_back(0);
}

LineGenerator::LineGenerator(::FILE* file) : input_type_(FILE) {
    file_input_.file = file;
    long initial_pos = ftell(file);
    line_starts_.push_back(static_cast<size_t>(initial_pos));
}

// 析构函数实现（无需特殊操作）
LineGenerator::~LineGenerator() {}

// 公有成员函数实现
std::string LineGenerator::next() {
    if (input_type_ == STRING) {
        return nextString();
    } else {
        return nextFile();
    }
}

bool LineGenerator::hasNext() const {
    if (input_type_ == STRING) {
        return str_input_.pos < str_input_.len;
    } else {
        return !feof(file_input_.file);
    }
}

void LineGenerator::rewindToPreviousLineStart() {
    if (line_starts_.size() < 2) return; // 不能回退到第一行之前
    line_starts_.pop_back();
    size_t prev_start = line_starts_.back();

    if (input_type_ == STRING) {
        str_input_.pos = prev_start;
    } else {
        fseek(file_input_.file, static_cast<long>(prev_start), SEEK_SET);
    }
}

// 私有成员函数实现
std::string LineGenerator::nextString() {
    size_t start_pos = line_starts_.back();
    str_input_.pos = start_pos;

    if (str_input_.pos >= str_input_.len) {
        return "";
    }

    std::string line;
    while (str_input_.pos < str_input_.len) {
        char c = str_input_.input[str_input_.pos++];
        if (c == '\r' && str_input_.pos < str_input_.len && str_input_.input[str_input_.pos] == '\n') {
            str_input_.pos++;
            break;
        } else if (c == '\n') {
            break;
        }
        line += c;
    }

    line_starts_.push_back(str_input_.pos);
    return line;
}

std::string LineGenerator::nextFile() {
    ::FILE *file = file_input_.file;
    long start_pos = static_cast<long>(line_starts_.back());
    fseek(file, start_pos, SEEK_SET);

    std::string line;
    int c;
    while ((c = fgetc(file)) != EOF) {
        if (c == '\r') {
            int next_c = fgetc(file);
            if (next_c != EOF && next_c != '\n') {
                ungetc(next_c, file);
            }
            break;
        } else if (c == '\n') {
            break;
        }
        line += static_cast<char>(c);
    }

    long next_pos = ftell(file);
    line_starts_.push_back(static_cast<size_t>(next_pos));
    return line;
}