#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <vector>
#include <string>
#include <unordered_map>
#include "token.h"
#include "value.h"
#include "lexer.h"
#include "keyword_process/keywords.h"

class Interpreter {
private:
    std::vector<Token> tokens;
    size_t pos = 0;

    Token advance();
    bool is_at_end() const;
    void handle_variable(const Token& token);

public:
    std::unordered_map<std::string, Value> symbol_table;

    void execute();
    Token get_next_token();
    Value evaluate_expression();
};

#endif // INTERPRETER_H