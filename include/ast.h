#ifndef AST_H
#define AST_H

#include <memory>
#include <string>
#include <vector>
#include "token.h"

// 前向声明
class Expr;
class Stmt;

// 表达式节点基类
class Expr {
public:
    virtual ~Expr() = default;
};

// 语句节点基类
class Stmt {
public:
    virtual ~Stmt() = default;
};

// 变量声明语句
class VarDeclaration : public Stmt {
public:
    Token type;
    Token name;
    std::unique_ptr<Expr> initializer;
    Token location_type;  // AT 或 IN
    Token location;      // 地址或块名

    VarDeclaration(Token type, Token name, std::unique_ptr<Expr> initializer,
                  Token location_type, Token location)
        : type(std::move(type)), name(std::move(name)),
          initializer(std::move(initializer)),
          location_type(std::move(location_type)),
          location(std::move(location)) {}
};

// 内存操作语句
class MemoryOp : public Stmt {
public:
    Token op_type;     // INC, MOVE, 或 COPY
    Token target;      // 操作目标
    Token destination; // 目标位置（对于MOVE和COPY）
    std::unique_ptr<Expr> value;  // 对于INC操作的值

    MemoryOp(Token op_type, Token target, Token destination, std::unique_ptr<Expr> value)
        : op_type(std::move(op_type)), target(std::move(target)),
          destination(std::move(destination)), value(std::move(value)) {}
};

// 打印和查找语句
class PrintFind : public Stmt {
public:
    Token op_type;  // PRINT 或 FIND
    Token target;   // 打印或查找的目标

    PrintFind(Token op_type, Token target)
        : op_type(std::move(op_type)), target(std::move(target)) {}
};

// 二元表达式
class Binary : public Expr {
public:
    std::unique_ptr<Expr> left;
    Token op;
    std::unique_ptr<Expr> right;

    Binary(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
        : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}
};

// 字面量表达式
class Literal : public Expr {
public:
    Token value;

    explicit Literal(Token value) : value(std::move(value)) {}
};

// 变量引用表达式
class Variable : public Expr {
public:
    Token name;

    explicit Variable(Token name) : name(std::move(name)) {}
};

// 函数调用表达式
class Call : public Expr {
public:
    Token callee;
    std::vector<std::unique_ptr<Expr>> arguments;

    Call(Token callee, std::vector<std::unique_ptr<Expr>> arguments)
        : callee(std::move(callee)), arguments(std::move(arguments)) {}
};

#endif // AST_H 