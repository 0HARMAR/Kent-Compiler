// token.h
#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <sstream>

enum class TokenType {
    // 字面量
    IDENTIFIER, STRING, INT, FLOAT, HEX,

    // 关键字
    TOKEN_SET, TOKEN_AT, TOKEN_MOVE, TOKEN_COPY,
    TOKEN_PRINT, TOKEN_PRINTLN, TOKEN_FIND,
    TOKEN_IN, TOKEN_BY, TOKEN_TO,

    // 数据类型
    TYPE_INT, TYPE_BYTE, TYPE_FLOAT,

    // 运算符
    PLUS, MINUS, MULTIPLY, DIVIDE,
    EQUAL, EQ_EQ, NOT_EQ,

    // 符号
    LPAREN, RPAREN, SEMI, COMMA,

    // 文件结束
    EOF_TYPE
};

struct Token {
    TokenType type;
    std::string lexeme;
    std::string literal; // 字面值
    int line;

    Token(TokenType t, const std::string& l, int ln)
            : type(t), lexeme(l), line(ln) {}

    Token(TokenType t, const std::string& l, const std::string& lit, int ln)
            : type(t), lexeme(l), literal(lit), line(ln) {}
            
    // 序列化token为ANTLR可读格式
    std::string serialize() const {
        std::stringstream ss;
        ss << line << "|" << static_cast<int>(type) << "|" << lexeme << "|" << literal;
        return ss.str();
    }
};

#endif // TOKEN_H