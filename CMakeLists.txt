cmake_minimum_required(VERSION 3.10)
project(kent C)

# 添加头文件目录
include_directories(include)

# 编译多个源文件
file(GLOB SOURCES "src/*.c")
add_executable(kentc ${SOURCES})