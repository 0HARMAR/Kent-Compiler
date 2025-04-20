# Kent 语言文档 v1.0

## 🚀 语言概览
**Kent** 是一种基于内存地址操作的轻量级语言，通过变量名直接管理内存空间
用户有一定大小的内存，用户可以像填格子一样在内存中填值

```kent
// 示例：存储并访问数据
set int counter = 0 at 0x300
print counter  // 输出: counter = 0 (0x300)