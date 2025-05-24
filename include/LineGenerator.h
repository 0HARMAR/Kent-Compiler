#ifndef LINE_GENERATOR_H
#define LINE_GENERATOR_H

#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

class LineGenerator {
public:
    enum InputType { STRING, FILE };

    // 构造函数声明
    LineGenerator(const char* input);
    LineGenerator(::FILE* file);

    // 析构函数声明
    ~LineGenerator();

    // 成员函数声明
    std::string next();
    bool hasNext() const;
    void rewindToPreviousLineStart();

private:
    enum InputType input_type_;

    // 匿名联合体声明
    union {
        struct {
            const char* input;
            size_t pos;
            size_t len;
        } str_input_;
        struct {
            ::FILE* file;
        } file_input_;
    };

    std::vector<size_t> line_starts_;

    // 私有成员函数声明
    std::string nextString();
    std::string nextFile();
};

#endif // LINE_GENERATOR_H