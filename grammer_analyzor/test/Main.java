import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.List;

public class Main {
    public static void main(String[] args) throws Exception {
        // 读取源文件
        String source = new String(Files.readAllBytes(
            Paths.get("/home/harmar/CLionProjects/Kent-Compiler/grammer_analyzor/test/test.kent")));
        
        // 词法分析
        Lexer lexer = new Lexer(source);
        List<Token> tokens = lexer.scan();
        
        // 打印token流
        System.out.println("Token Stream:");
        for (Token token : tokens) {
            System.out.printf("Line %d: %s '%s'%n", 
                token.line, token.type, token.lexeme);
        }
        
        // 语法分析
        Parser parser = new Parser(tokens);
        List<Stmt> statements = parser.parse();
        
        // 打印AST
        System.out.println("\nAbstract Syntax Tree:");
        for (Stmt stmt : statements) {
            System.out.println(stmt.toString());
        }
    }
}