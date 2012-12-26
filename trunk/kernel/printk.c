/**
 * 内核打印函数
 */
#include <stdarg.h>

static char buf[1024];
extern int vsprintf(char * buf, const char * fmt, va_list args);

int printk(const char *fmt, ...) {
	va_list args;
	//将args指向fmt后的第一个参数
	va_start(args, fmt);
	//初始化参数列表
	va_end(args);
	int i = vsprintf(buf,fmt, args);
	//TOTO 调用tty_write函数
	return i;
}
