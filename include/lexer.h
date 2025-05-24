// lexer.h
#ifndef LEXER_H
#define LEXER_H

#include <vector>
#include <string>
#include "token.h"
#include "keyword_process/keywords.h"

class Lexer {
private:
    std::string source;
    size_t current_pos = 0;
    int line = 1;

    void skip_whitespace();
    Token scan_string();
    Token scan_number();
    Token scan_identifier();
    Token scan_operator();

public:
    explicit Lexer(const std::string& input) : source(input) {}

    std::vector<Token> scan();
    void output_tokens(const std::string& output_file);
    static const std::unordered_map<std::string, TokenType>& keywords();
};

#endif // LEXER_H