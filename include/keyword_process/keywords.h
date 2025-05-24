//
// Created by hemingyang on 2025/5/3.
//

#ifndef KEYWORDS_H
#define KEYWORDS_H

#include <string_view>
#include <unordered_map>
#include "../token.h"

class LexerMap {
public:
    static const std::unordered_map<std::string_view, TokenType>& keyword_map() {
        static const std::unordered_map<std::string_view, TokenType> kw = {
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
};

#endif // KEYWORDS_H
