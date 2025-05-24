import java.util.ArrayList;
import java.util.List;

public class Parser {
    private final List<Token> tokens;
    private int current = 0;

    public Parser(List<Token> tokens) {
        this.tokens = tokens;
    }

    private Token peek() {
        return tokens.get(current);
    }

    private Token previous() {
        return tokens.get(current - 1);
    }

    private Token advance() {
        if (!isAtEnd()) current++;
        return previous();
    }

    private boolean isAtEnd() {
        return peek().type == TokenType.EOF_TYPE;
    }

    private boolean check(TokenType type) {
        if (isAtEnd()) return false;
        return peek().type == type;
    }

    private boolean match(TokenType type) {
        if (check(type)) {
            advance();
            return true;
        }
        return false;
    }

    private Token consume(TokenType type, String message) {
        if (check(type)) return advance();
        throw new RuntimeException(message);
    }

    public List<Stmt> parse() {
        List<Stmt> statements = new ArrayList<>();
        while (!isAtEnd()) {
            statements.add(statement());
        }
        return statements;
    }

    private Stmt statement() {
        if (match(TokenType.SET)) return varDeclaration();
        if (match(TokenType.INC) || match(TokenType.MOVE) || match(TokenType.COPY)) return memoryOp();
        if (match(TokenType.PRINT) || match(TokenType.FIND)) return printFind();
        return exprStatement();
    }

    private Stmt varDeclaration() {
        Token type = consume(TokenType.INT, "Expect type after 'set'.");
        Token name = consume(TokenType.IDENTIFIER, "Expect variable name.");
        
        Expr initializer = null;
        if (match(TokenType.EQUAL)) {
            initializer = expression();
        }
        
        Token locType = consume(TokenType.AT, "Expect 'at' or 'in' after variable declaration.");
        Token location = consume(TokenType.NUMBER, "Expect memory address or block name.");
        consume(TokenType.SEMI, "Expect ';' after variable declaration.");
        
        return new Stmt.VarDeclaration(type, name, initializer, locType, location);
    }

    private Stmt memoryOp() {
        Token opType = previous();
        Token target = consume(TokenType.IDENTIFIER, "Expect target identifier.");
        
        if (opType.type == TokenType.INC) {
            consume(TokenType.BY, "Expect 'by' after increment target.");
            Expr value = expression();
            consume(TokenType.SEMI, "Expect ';' after increment.");
            return new Stmt.MemoryOp(opType, target, null, value);
        }
        
        consume(TokenType.TO, "Expect 'to' after source.");
        Token dest = consume(TokenType.IDENTIFIER, "Expect destination identifier.");
        consume(TokenType.SEMI, "Expect ';' after memory operation.");
        
        return new Stmt.MemoryOp(opType, target, dest, null);
    }

    private Stmt printFind() {
        Token opType = previous();
        Token target = consume(TokenType.IDENTIFIER, "Expect identifier.");
        consume(TokenType.SEMI, "Expect ';' after print/find.");
        return new Stmt.PrintFind(opType, target);
    }

    private Stmt exprStatement() {
        Expr expr = expression();
        consume(TokenType.SEMI, "Expect ';' after expression.");
        return new Stmt.ExprStmt(expr);
    }

    private Expr expression() {
        return term();
    }

    private Expr term() {
        Expr expr = factor();
        
        while (match(TokenType.PLUS) || match(TokenType.MINUS)) {
            Token operator = previous();
            Expr right = factor();
            expr = new Expr.Binary(expr, operator, right);
        }
        
        return expr;
    }

    private Expr factor() {
        Expr expr = primary();
        
        while (match(TokenType.MULTIPLY) || match(TokenType.DIVIDE)) {
            Token operator = previous();
            Expr right = primary();
            expr = new Expr.Binary(expr, operator, right);
        }
        
        return expr;
    }

    private Expr primary() {
        if (match(TokenType.NUMBER)) return new Expr.Literal(previous());
        if (match(TokenType.IDENTIFIER)) {
            Token name = previous();
            if (match(TokenType.LPAREN)) {
                List<Expr> arguments = new ArrayList<>();
                if (!check(TokenType.RPAREN)) {
                    do {
                        arguments.add(expression());
                    } while (match(TokenType.COMMA));
                }
                consume(TokenType.RPAREN, "Expect ')' after arguments.");
                return new Expr.Call(name, arguments);
            }
            return new Expr.Variable(name);
        }
        
        if (match(TokenType.LPAREN)) {
            Expr expr = expression();
            consume(TokenType.RPAREN, "Expect ')' after expression.");
            return expr;
        }
        
        throw new RuntimeException("Expect expression.");
    }
} 