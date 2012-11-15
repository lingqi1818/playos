/**
 * 本程序主要用于对参数产生格式化的输出
 */
#include <stdarg.h>
#define is_digit(c)	((c) >= '0' && (c) <= '9')

static int skip_atoi(const **s){
	int i=0;

		while (is_digit(**s))
			i = i*10 + *((*s)++) - '0';
		return i;
}


#define ZEROPAD	1		/* pad with zero */
#define SIGN	2		/* unsigned/signed long */
#define PLUS	4		/* show plus */
#define SPACE	8		/* space if plus */
#define LEFT	16		/* left justified */
#define SPECIAL	32		/* 0x */
#define SMALL	64		/* use 'abcdef' instead of 'ABCDEF' */

//大括号表达式集合，值为最后一个表达式
#define do_div(n,base) ({ \
int __res; \
__asm__("divl %4":"=a" (n),"=d" (__res):"0" (n),"1" (0),"r" (base)); \
__res; })

/**
 *将整数转换成指定进制的字符串
 *str：转换后的字符串
 *num:需要转换的整数
 *base:进制
 *size:整数部分长度
 *precision：数字精度
 *type:flag类型
 */
static char * number(char * str, int num, int base, int size, int precision,int type)
{
	char c,sign,tmp[36];
	int i;
	const char *digits="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	if (type&SMALL) digits="0123456789abcdefghijklmnopqrstuvwxyz";
	if (type&LEFT) type &= ~ZEROPAD;//如果类型为左对齐，则屏蔽填0标志位
	if (base<2 || base>36)//不支持的进制
	return 0;
	c = (type & ZEROPAD) ? '0' : ' ' ;
	if (type&SIGN && num<0) {//判断标志符为：+，-或' '.
		sign='-';
		num = -num;//负数
	} else
		sign=(type&PLUS) ? '+' : ((type&SPACE) ? ' ' : 0);
	if (sign) size--; //符号不为0
	if (type&SPECIAL)//特殊类型符号
			if (base==16) size -= 2;//0x size-2
			else if (base==8) size--;//0 size-1
	i=0;
	if (num==0)
		tmp[i++]='0';
	else while (num!=0)
		tmp[i++]=digits[do_div(num,base)];
	if (i>precision) precision=i;
		size -= precision;
	if (!(type&(ZEROPAD+LEFT)))//不是左对齐，且补空格
		while(size-->0)
			*str++ = ' ';
	if (sign)//符号
		*str++ = sign;
	if (type&SPECIAL)
		if (base==8)
			*str++ = '0';
		else if (base==16) {
			*str++ = '0';
			*str++ = digits[33];
		}
	if (!(type&LEFT))
		while(size-->0)
			*str++ = c;
	while(i<precision--)
		*str++ = '0';
	while(i-->0)
		*str++ = tmp[i];
	while(size-->0)
		*str++ = ' ';
	return str;
}

int vsprintf(char *buf, const char *fmt, va_list args){
	int *ip;
	int len;
	int i;
	char *s;
	char *str;
	int flags;
	int field_width;
	int precision;
	int qualifier;
	for (str=buf ; *fmt ; ++fmt) {
		if (*fmt != '%') {
			*str++ = *fmt;
			continue;
		}
	repeat:
	flags=0;
	++fmt;//skip '%'
	switch (*fmt) {
		case '-': flags |= LEFT; goto repeat;
		case '+': flags |= PLUS; goto repeat;
		case ' ': flags |= SPACE; goto repeat;
		case '#': flags |= SPECIAL; goto repeat;
		case '0': flags |= ZEROPAD; goto repeat;
		}
	field_width = -1;
	if (is_digit(*fmt))//如果为数字，则直接取数字，如果为*则取下个参数值
		field_width = skip_atoi(&fmt);
	else if (*fmt == '*') {
		field_width = va_arg(args, int);
		if (field_width < 0) {
			field_width = -field_width;
			flags |= LEFT;
		}
	}

	/* get the precision */
		precision = -1;
		if (*fmt == '.') {
			++fmt;
			if (is_digit(*fmt))
				precision = skip_atoi(&fmt);
			else if (*fmt == '*') {
				/* it's the next argument */
				precision = va_arg(args, int);
			}
			if (precision < 0)
				precision = 0;
		}

		/* get the conversion qualifier */
		qualifier = -1;
		if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L') {
			qualifier = *fmt;
			++fmt;
		}


		switch (*fmt) {
			case 'c':
					if (!(flags & LEFT))//不是左对齐，则在前面补空格，否则在后面补空格
						while (--field_width > 0)
							*str++ = ' ';
					*str++ = (unsigned char) va_arg(args, int);
					while (--field_width > 0)
						*str++ = ' ';
					break;

			 case 's':
					s = va_arg(args, char *);
					len = strlen(s);
					if (precision < 0)
						precision = len;
					else if (len > precision)
						len = precision;

					if (!(flags & LEFT))
						while (len < field_width--)
							*str++ = ' ';
					for (i = 0; i < len; ++i)
						*str++ = *s++;
					while (len < field_width--)
						*str++ = ' ';
					break;

			 case 'o':
					str = number(str, va_arg(args, unsigned long), 8,
						field_width, precision, flags);
					break;
			 case 'p'://指针类型
					if (field_width == -1) {
						field_width = 8;
						flags |= ZEROPAD;
					}
					str = number(str,
						(unsigned long) va_arg(args, void *), 16,
						field_width, precision, flags);
					break;
			 case 'x'://16进制
					flags |= SMALL;
				case 'X':
					str = number(str, va_arg(args, unsigned long), 16,
						field_width, precision, flags);
					break;
			case 'd'://整数
			case 'i'://有符号
				flags |= SIGN;
			case 'u'://无符号
				str = number(str, va_arg(args, unsigned long), 10,
					field_width, precision, flags);
				break;
			case 'n'://停止打印，并将已经打印的字符数写入到int指针指向的地址
				ip = va_arg(args, int *);
				*ip = (str - buf);
				break;
			default:
				if (*fmt != '%')
					*str++ = '%';
				if (*fmt)
					*str++ = *fmt;
				else
					--fmt;
				break;
		}

	}
	*str = '\0';
	return str-buf;
}
