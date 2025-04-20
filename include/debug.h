// 带颜色、文件名、行号、函数名
#define DEBUG_LOG(fmt, ...) \
    printf("\033[36m[DEBUG] %s:%d <%s>\033[0m " fmt "\n", \
           __FILE__, __LINE__, __func__, ##__VA_ARGS__)