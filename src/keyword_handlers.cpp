// handlers.cpp
#include <iostream>
#include "interpreter.h" // 包含解释器上下文定义

void handle_set(Interpreter& ctx) {
    // 示例：处理 set a = 5
    auto var_name = ctx.get_next_token();
    ctx.expect(TOKEN_ASSIGN);
    auto value = ctx.evaluate_expression();
    ctx.symbol_table[var_name.lexeme] = value;
}

void handle_print(Interpreter& ctx) {
    while(auto expr = ctx.peek_next()) {
        auto val = ctx.evaluate_expression();
        std::cout << val;
        if(ctx.peek_current() != TOKEN_COMMA) break;
    }
}

void handle_println(Interpreter& ctx) {
    handle_print(ctx);
    std::cout << "\n";
}

// 其他处理函数类似...