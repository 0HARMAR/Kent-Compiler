//
// Created by hemingyang on 2025/5/3.
//

#include <vector>
#include <string>
#include "interpreter.h"
#include "keyword_process/keywords.h"

void Interpreter::execute() {
        while(!is_at_end()) {
            auto token = advance();

            if(token.type == TokenType::SYMBOL) {
                // 处理变量访问
                handle_variable(token);
            }
            else if(auto it = LexerMap::handler_map().find(token.type);
                    it != LexerMap::handler_map().end())
            {
                // 执行关键字处理函数
                it->second(*this);
            }
            else {
                throw SyntaxError("Unexpected token");
            }
        }
}

Token Interpreter::get_next_token() { /* ... */ }
Value Interpreter::evaluate_expression() { /* ... */ }