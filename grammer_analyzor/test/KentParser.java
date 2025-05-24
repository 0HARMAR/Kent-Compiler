import org.antlr.v4.runtime.*;
import org.antlr.v4.runtime.tree.*;
import java.nio.file.*;

public class KentParser {
    public static void main(String[] args) throws Exception {
        if (args.length != 2) {
            System.err.println("Usage: KentParser <source-file> <tokens-file>");
            System.exit(1);
        }

        String sourceFile = args[0];
        String tokensFile = args[1];

        // 创建词法分析器并加载token
        KentLexer lexer = new KentLexer(null);
        lexer.loadTokens(tokensFile);

        // 创建token流
        CommonTokenStream tokens = new CommonTokenStream(lexer);

        // 创建语法分析器
        KentParser parser = new KentParser(tokens);
        
        // 开始解析
        ParseTree tree = parser.program();

        // 打印语法树
        System.out.println(tree.toStringTree(parser));
    }
} 