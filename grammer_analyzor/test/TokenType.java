public enum TokenType {
    // 字面量
    IDENTIFIER, NUMBER, STRING,
    
    // 关键字
    SET, INT, BYTE, FLOAT, AT, IN,
    INC, BY, MOVE, TO, COPY,
    PRINT, FIND,
    
    // 运算符和分隔符
    PLUS, MINUS, MULTIPLY, DIVIDE,
    EQUAL, SEMI, COMMA, 
    LPAREN, RPAREN,
    
    EOF_TYPE
} 