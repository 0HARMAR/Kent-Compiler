//
// Created by hemingyang on 2025/5/5.
//

#ifndef KENT_TYPE_H
#define KENT_TYPE_H
// 数据类型标记（基础类型扩展用）
typedef enum {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_CHAR,
    TYPE_POINTER,
    TYPE_ARRAY,
    TYPE_CUSTOM      // 用户定义类型
} DataType;
#endif //KENT_TYPE_H
