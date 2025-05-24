cmake_minimum_required(VERSION 3.10)
project(kent CXX)

# 添加头文件目录
include_directories(include)

# 编译词法分析器测试程序
add_executable(test_lexer 
    test_lexer.cpp
    src/lexer.cpp
)

# 编译主程序
file(GLOB SOURCES "src/*.cpp")
add_executable(kentc ${SOURCES}
        src/LineGenerator.cpp
        include/LineGenerator.h
        src/gen_print_asm.cpp
        include/gen_print_asm.h
        src/gen_header_footer.cpp
        include/gen_header_footer.h
        src/extract_operation_block.cpp
        include/extract_operation_block.h
        include/keyword_process/keywords.h
        src/keyword_handlers.cpp
        src/interpreter.cpp
        include/interpreter.h
        src/lexer.cpp
        include/lexer.h
        include/token.h
        include/symbol.h
        include/scope.h
        include/type.h
        include/scope_stack.h)