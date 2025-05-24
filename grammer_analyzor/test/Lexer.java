import java.util.ArrayList;
import java.util.List;

public class Lexer {
    private final String source;
    private int current = 0;
    private int line = 1;

    public Lexer(String source) {
        this.source = source;
    }

    private void skipWhitespace() {
        while (current < source.length()) {
            char c = source.charAt(current);
            if (c == '\n') line++;
            if (!Character.isWhitespace(c)) break;
            current++;
        }
    }

    private Token scanNumber() {
        StringBuilder num = new StringBuilder();
        while (current < source.length() && Character.isDigit(source.charAt(current))) {
            num.append(source.charAt(current++));
        }
        return new Token(TokenType.NUMBER, num.toString(), line);
    }

    private Token scanIdentifier() {
        StringBuilder id = new StringBuilder();
        while (current < source.length() && 
               (Character.isLetterOrDigit(source.charAt(current)) || source.charAt(current) == '_')) {
            id.append(source.charAt(current++));
        }
        String text = id.toString();
        TokenType type = keywords.getOrDefault(text, TokenType.IDENTIFIER);
        return new Token(type, text, line);
    }

    private Token scanOperator() {
        char c = source.charAt(current++);
        return new Token(TokenType.EQUAL, String.valueOf(c), line);
    }

    public List<Token> scan() {
        List<Token> tokens = new ArrayList<>();
        while (current < source.length()) {
            skipWhitespace();
            if (current >= source.length()) break;

            char c = source.charAt(current);
            if (Character.isDigit(c)) {
                tokens.add(scanNumber());
            } else if (Character.isLetter(c) || c == '_') {
                tokens.add(scanIdentifier());
            } else {
                tokens.add(scanOperator());
            }
        }
        tokens.add(new Token(TokenType.EOF_TYPE, "", line));
        return tokens;
    }

    private static final java.util.Map<String, TokenType> keywords = new java.util.HashMap<>();
    static {
        keywords.put("set", TokenType.SET);
        keywords.put("int", TokenType.INT);
        keywords.put("byte", TokenType.BYTE);
        keywords.put("float", TokenType.FLOAT);
        keywords.put("at", TokenType.AT);
        keywords.put("in", TokenType.IN);
        keywords.put("inc", TokenType.INC);
        keywords.put("by", TokenType.BY);
        keywords.put("move", TokenType.MOVE);
        keywords.put("to", TokenType.TO);
        keywords.put("copy", TokenType.COPY);
        keywords.put("print", TokenType.PRINT);
        keywords.put("find", TokenType.FIND);
    }
} 