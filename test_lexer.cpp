#include "lexer.h"
#include <iostream>
#include <fstream>

int main() {
    std::string test_content;
    {
        std::ifstream in("test.kent");
        if (!in) {
            std::cerr << "Failed to open test.kent\n";
            return 1;
        }
        std::string line;
        while (std::getline(in, line)) {
            test_content += line + "\n";
        }
    }

    Lexer lexer(test_content);
    lexer.output_tokens("tokens.txt");
    
    std::cout << "Tokens have been written to tokens.txt\n";
    return 0;
} 