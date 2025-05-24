public abstract class Stmt {
    public static class VarDeclaration extends Stmt {
        final Token type;
        final Token name;
        final Expr initializer;
        final Token locationType;
        final Token location;

        VarDeclaration(Token type, Token name, Expr initializer,
                      Token locationType, Token location) {
            this.type = type;
            this.name = name;
            this.initializer = initializer;
            this.locationType = locationType;
            this.location = location;
        }

        @Override
        public String toString() {
            String init = initializer != null ? " = " + initializer : "";
            return String.format("set %s %s%s %s %s;",
                type.lexeme, name.lexeme, init,
                locationType.lexeme, location.lexeme);
        }
    }

    public static class MemoryOp extends Stmt {
        final Token opType;
        final Token target;
        final Token destination;
        final Expr value;

        MemoryOp(Token opType, Token target, Token destination, Expr value) {
            this.opType = opType;
            this.target = target;
            this.destination = destination;
            this.value = value;
        }

        @Override
        public String toString() {
            if (opType.type == TokenType.INC) {
                return String.format("inc %s by %s;", target.lexeme, value);
            }
            return String.format("%s %s to %s;",
                opType.lexeme, target.lexeme, destination.lexeme);
        }
    }

    public static class PrintFind extends Stmt {
        final Token opType;
        final Token target;

        PrintFind(Token opType, Token target) {
            this.opType = opType;
            this.target = target;
        }

        @Override
        public String toString() {
            return String.format("%s %s;", opType.lexeme, target.lexeme);
        }
    }

    public static class ExprStmt extends Stmt {
        final Expr expression;

        ExprStmt(Expr expression) {
            this.expression = expression;
        }

        @Override
        public String toString() {
            return expression.toString() + ";";
        }
    }
} 