/**
 * 内核打印函数
 */
#include <stdarg.h>

static char buf[1024];
extern int vsprintf(char * buf, const char * fmt, va_list args);
