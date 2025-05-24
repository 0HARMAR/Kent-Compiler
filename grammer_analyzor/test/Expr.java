import java.util.List;

public abstract class Expr {
    public static class Binary extends Expr {
        final Expr left;
        final Token operator;
        final Expr right;

        Binary(Expr left, Token operator, Expr right) {
            this.left = left;
            this.operator = operator;
            this.right = right;
        }

        @Override
        public String toString() {
            return String.format("(%s %s %s)", left, operator.lexeme, right);
        }
    }

    public static class Literal extends Expr {
        final Token value;

        Literal(Token value) {
            this.value = value;
        }

        @Override
        public String toString() {
            return value.lexeme;
        }
    }

    public static class Variable extends Expr {
        final Token name;

        Variable(Token name) {
            this.name = name;
        }

        @Override
        public String toString() {
            return name.lexeme;
        }
    }

    public static class Call extends Expr {
        final Token callee;
        final List<Expr> arguments;

        Call(Token callee, List<Expr> arguments) {
            this.callee = callee;
            this.arguments = arguments;
        }

        @Override
        public String toString() {
            StringBuilder sb = new StringBuilder();
            sb.append(callee.lexeme).append("(");
            for (int i = 0; i < arguments.size(); i++) {
                if (i > 0) sb.append(", ");
                sb.append(arguments.get(i));
            }
            sb.append(")");
            return sb.toString();
        }
    }
} 