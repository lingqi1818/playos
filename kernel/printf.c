/**
 * 向终端输出格式化的字符串
 */
#include <stdarg.h>
static char printbuf[1024];

int printf(const char *fmt, ...)
{
	va_list args;
	int i;

	va_start(args, fmt);
	write(1,printbuf,i=vsprintf(printbuf, fmt, args));
	va_end(args);
	return i;
}
