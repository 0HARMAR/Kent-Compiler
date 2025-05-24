#include <stdint.h>  // 定义 uint64_t
#include <string>
#include "scope.h"
#include "type.h"

using namespace std;
typedef struct {
    char name[32];
    uint64_t offset;  // 相对于基地址的偏移量
    int size;
    char value[32];
    Scope scope;
    DataType type;
} Variable;