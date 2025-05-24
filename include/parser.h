#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <memory>
#include "token.h"
#include "ast.h"

class Parser {
private:
    std::vector<Token> tokens;
    size_t current = 0;

    // 辅助方法
    Token peek() const;
    Token previous() const;
    Token advance();
    bool isAtEnd() const;
    bool check(TokenType type) const;
    bool match(TokenType type);
    Token consume(TokenType type, const std::string& message);
    
    // 递归下降解析方法
    std::unique_ptr<Stmt> statement();
    std::unique_ptr<Stmt> varDeclaration();
    std::unique_ptr<Stmt> memoryOp();
    std::unique_ptr<Stmt> printFind();
    std::unique_ptr<Stmt> exprStatement();
    std::unique_ptr<Expr> expression();
    std::unique_ptr<Expr> term();
    std::unique_ptr<Expr> factor();
    std::unique_ptr<Expr> primary();

public:
    explicit Parser(std::vector<Token> tokens) : tokens(std::move(tokens)) {}
    std::vector<std::unique_ptr<Stmt>> parse();
};

#endif // PARSER_H 