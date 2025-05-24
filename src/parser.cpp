#include "parser.h"
#include <stdexcept>

Token Parser::peek() const {
    return tokens[current];
}

Token Parser::previous() const {
    return tokens[current - 1];
}

Token Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

bool Parser::isAtEnd() const {
    return peek().type == TokenType::EOF_TYPE;
}

bool Parser::check(TokenType type) const {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    throw std::runtime_error(message);
}

std::vector<std::unique_ptr<Stmt>> Parser::parse() {
    std::vector<std::unique_ptr<Stmt>> statements;
    while (!isAtEnd()) {
        statements.push_back(statement());
    }
    return statements;
}

std::unique_ptr<Stmt> Parser::statement() {
    if (match(TokenType::SET)) {
        return varDeclaration();
    }
    if (match(TokenType::INC) || match(TokenType::MOVE) || match(TokenType::COPY)) {
        return memoryOp();
    }
    if (match(TokenType::PRINT) || match(TokenType::FIND)) {
        return printFind();
    }
    return exprStatement();
}

std::unique_ptr<Stmt> Parser::varDeclaration() {
    Token type = consume(TokenType::INT, "Expect type after 'set'.");
    Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");
    
    std::unique_ptr<Expr> initializer;
    if (match(TokenType::EQUAL)) {
        initializer = expression();
    }
    
    Token locType = consume(TokenType::AT, "Expect 'at' or 'in' after variable declaration.");
    Token location = consume(TokenType::HEX, "Expect memory address or block name.");
    
    consume(TokenType::SEMI, "Expect ';' after variable declaration.");
    
    return std::make_unique<VarDeclaration>(type, name, std::move(initializer), locType, location);
}

std::unique_ptr<Stmt> Parser::memoryOp() {
    Token opType = previous();
    Token target = consume(TokenType::IDENTIFIER, "Expect target identifier.");
    
    if (opType.type == TokenType::INC) {
        consume(TokenType::BY, "Expect 'by' after increment target.");
        auto value = expression();
        consume(TokenType::SEMI, "Expect ';' after increment.");
        return std::make_unique<MemoryOp>(opType, target, Token{}, std::move(value));
    }
    
    consume(TokenType::TO, "Expect 'to' after source.");
    Token dest = consume(TokenType::IDENTIFIER, "Expect destination identifier.");
    consume(TokenType::SEMI, "Expect ';' after memory operation.");
    
    return std::make_unique<MemoryOp>(opType, target, dest, nullptr);
}

std::unique_ptr<Stmt> Parser::printFind() {
    Token opType = previous();
    Token target = consume(TokenType::IDENTIFIER, "Expect identifier.");
    consume(TokenType::SEMI, "Expect ';' after print/find.");
    return std::make_unique<PrintFind>(opType, target);
}

std::unique_ptr<Expr> Parser::expression() {
    auto expr = term();
    
    while (match(TokenType::PLUS) || match(TokenType::MINUS)) {
        Token op = previous();
        auto right = term();
        expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expr> Parser::term() {
    auto expr = factor();
    
    while (match(TokenType::MULTIPLY) || match(TokenType::DIVIDE)) {
        Token op = previous();
        auto right = factor();
        expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expr> Parser::factor() {
    if (match(TokenType::NUMBER)) {
        return std::make_unique<Literal>(previous());
    }
    
    if (match(TokenType::IDENTIFIER)) {
        Token name = previous();
        if (match(TokenType::LPAREN)) {
            std::vector<std::unique_ptr<Expr>> arguments;
            if (!check(TokenType::RPAREN)) {
                do {
                    arguments.push_back(expression());
                } while (match(TokenType::COMMA));
            }
            consume(TokenType::RPAREN, "Expect ')' after arguments.");
            return std::make_unique<Call>(name, std::move(arguments));
        }
        return std::make_unique<Variable>(name);
    }
    
    if (match(TokenType::LPAREN)) {
        auto expr = expression();
        consume(TokenType::RPAREN, "Expect ')' after expression.");
        return expr;
    }
    
    throw std::runtime_error("Expect expression.");
} 