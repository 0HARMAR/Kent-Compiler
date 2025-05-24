# Kent 语言手册

## 🌩️ 核心特性
- **内存直连** - 变量直接绑定物理地址
- **显式类型** - `int` (32位) / `float` (32位) / `byte` (8位)
- **硬件级操作** - 支持位运算与内存块拷贝
- **零运行时** - 无垃圾回收或动态内存分配

## 💡 重要概念
- **操作块** - 用大括号包裹起来的操作集合，例如循环块，函数块都是操作块
- **数据区** - 一段连续的内存空间，用户可以为数据区命名

## 🔧 基础操作BO

### 变量声明与赋值
```c
// 固定地址声明
set int counter = 0 at 0x3000
set byte flags = 0b1010 at 0x200
//随机地址声明
set int a = 111
// 赋值
counter  = 1

```

### 内存操作
- move src dst,将源地址数据/数据移动到目标地址，源地址数据清0
- copy src dst,将源地址数据复制到目标地址，原地址数据不变
```c
move 0xFF to 0x3000     
move 11 to 0x300
move var to var
move 1 to var

copy 0x10 to 0x20
```

### print & find
```c
print var_name
find var_name/mem_addr
```

### 算术操作
```c
//加减乘除
a = 1+2*3+10/5
```

### 函数调用
```c
a = add(1,3) + sub(3,1)
```
## 🎮 逻辑操作LO

```c
// 嵌套的循环器与局部数据
looper(5){
    set int a = 1 at 0x200
    looper(5){
        a += 1
    }        
}
```

```c
switcher a
    case 1:
        println "1"
    case 2:
        println "2"
```

## 🛠️ 函数操作FO

```c
fun 
```

## 🐱 操作块
- 可以为操作块分配数据区，存储局部变量
```c
dataspace Demo using 0x10 to 0x100
demo apply Demo{
    set bool is_key = true
    copy is_key to 0x0
    move 0x10 to 0x20
    print 0x20
    looper(10){
        static int counter = 0
        set int addr = 0x30 + counter        
        set counter at addr
        counter++
    }
}
```

## 🌈 数据区
```c
dataspace file using 0x100 to 0x1000
map "./demo.txt" at file
```

## 变量类型及修饰符
- 全局变量，默认声明的变量为全局变量
- 局部变量，局部变量作用域为所声明的操作块
- 静态变量，变量一经声明便存活到程序结束，作用域为声明操作块作用域，参考c语言静态变量
- 代理变量，变量存储在编译器内部，由编译器代理管理，不在用户数据区中存储，常用于函数中的临时变量

- local 声明局部变量
- static 声明静态变量
- proxy 声明代理变量

# Kent Language Manual

## 🌩️ Core Features
- **Direct Memory Mapping** - Variables bound to physical addresses
- **Explicit Typing** - `int` (32-bit) / `float` (32-bit) / `byte` (8-bit)
- **Hardware-Level Operations** - Bitwise operations & memory block copying
- **Zero Overhead Runtime** - No garbage collection or dynamic memory allocation

## 💡 Key Concepts
- **Operation Blocks** - Curly brace-enclosed code segments (e.g., `{ loops }`, `{ functions }`)
- **Data Sections** - Contiguous memory regions with user-defined names

## 🔧 Basic Syntax

### Variable Declaration
```c
// Fixed-address declaration
set int counter = 0 at 0x3000
set byte flags = 0b1010 at 0x200

// Memory section allocation
memory BLOCK_A {
    start: 0x8000
    size: 4KB
}
set float sensor_data[10] in BLOCK_A
```