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
	__asm__("push %%fs\n\t"
			"push %%ds\n\t"
			"pop %%fs\n\t"
			"pushl %0\n\t"
			"pushl $_buf\n\t"
			"pushl $0\n\t"
			"call tty_write\n\t"
			"addl $8,%%esp\n\t"
			"popl %0\n\t"
			"pop %%fs"
			::"r" (i):"ax","cx","dx");
	return i;
}
