// lexer.cpp
#include "lexer.h"
#include <cctype>
#include <unordered_map>
#include <stdexcept>
#include <fstream>

using namespace std;

const unordered_map<string, TokenType>& Lexer::keywords() {
    static const unordered_map<string, TokenType> kw = {
        {"set", TokenType::TOKEN_SET},
        {"at", TokenType::TOKEN_AT},
        {"in", TokenType::TOKEN_IN},
        {"move", TokenType::TOKEN_MOVE},
        {"to", TokenType::TOKEN_TO},
        {"copy", TokenType::TOKEN_COPY},
        {"print", TokenType::TOKEN_PRINT},
        {"println", TokenType::TOKEN_PRINTLN},
        {"find", TokenType::TOKEN_FIND},
        {"by", TokenType::TOKEN_BY},
        {"int", TokenType::TYPE_INT},
        {"byte", TokenType::TYPE_BYTE},
        {"float", TokenType::TYPE_FLOAT}
    };
    return kw;
}

void Lexer::output_tokens(const string& output_file) {
    vector<Token> tokens = scan();
    ofstream out(output_file);
    for (const auto& token : tokens) {
        out << token.serialize() << "\n";
    }
}

Token Lexer::scan_string() {
    string value;
    char quote = source[current_pos++];

    while (current_pos < source.size() && source[current_pos] != quote) {
        if (source[current_pos] == '\\') { // 处理转义字符
            current_pos++;
        }
        value += source[current_pos++];
    }

    if (current_pos >= source.size()) {
        throw runtime_error("Unterminated string at line " + to_string(line));
    }

    current_pos++; // 跳过闭合引号
    return {TokenType::STRING, value, line};
}

Token Lexer::scan_number() {
    string num;
    bool has_dot = false;

    // 检查是否是十六进制
    if (current_pos + 1 < source.size() && source[current_pos] == '0' && 
        (source[current_pos + 1] == 'x' || source[current_pos + 1] == 'X')) {
        num = "0x";
        current_pos += 2;
        while (current_pos < source.size() && isxdigit(source[current_pos])) {
            num += source[current_pos++];
        }
        return {TokenType::HEX, num, line};
    }

    while (current_pos < source.size()) {
        char c = source[current_pos];
        if (isdigit(c)) {
            num += c;
            current_pos++;
        } else if (c == '.' && !has_dot) {
            num += c;
            has_dot = true;
            current_pos++;
        } else {
            break;
        }
    }

    return {has_dot ? TokenType::FLOAT : TokenType::INT, num, line};
}

Token Lexer::scan_identifier() {
    string text;
    while (current_pos < source.size()) {
        char c = source[current_pos];
        if (isalnum(c) || c == '_') {
            text += c;
            current_pos++;
        } else {
            break;
        }
    }

    // 检查关键字
    auto it = keywords().find(text);
    return {
        it != keywords().end() ? it->second : TokenType::IDENTIFIER,
        text,
        line
    };
}

Token Lexer::scan_operator() {
    char c = source[current_pos++];
    string op(1, c);

    switch (c) {
        case '+': return {TokenType::PLUS, op, line};
        case '-': return {TokenType::MINUS, op, line};
        case '*': return {TokenType::MULTIPLY, op, line};
        case '/': return {TokenType::DIVIDE, op, line};
        case '(': return {TokenType::LPAREN, op, line};
        case ')': return {TokenType::RPAREN, op, line};
        case ';': return {TokenType::SEMI, op, line};
        case ',': return {TokenType::COMMA, op, line};
        case '=':
            if (current_pos < source.size() && source[current_pos] == '=') {
                current_pos++;
                return {TokenType::EQ_EQ, "==", line};
            }
            return {TokenType::EQUAL, "=", line};
        default:
            throw runtime_error("Unexpected operator: " + op);
    }
}

void Lexer::skip_whitespace() {
    while (current_pos < source.size()) {
        char c = source[current_pos];
        if (c == '\n') line++;
        if (!isspace(c)) break;
        current_pos++;
    }
}

vector<Token> Lexer::scan() {
    vector<Token> tokens;

    while (current_pos < source.size()) {
        skip_whitespace();
        if (current_pos >= source.size()) break;

        char c = source[current_pos];
        if (c == '"' || c == '\'') {
            tokens.push_back(scan_string());
        } else if (isdigit(c)) {
            tokens.push_back(scan_number());
        } else if (isalpha(c) || c == '_') {
            tokens.push_back(scan_identifier());
        } else if (ispunct(c)) {
            tokens.push_back(scan_operator());
        } else {
            throw runtime_error("Unexpected character: " + string(1, c));
        }
    }

    tokens.emplace_back(TokenType::EOF_TYPE, "", line);
    return tokens;
}