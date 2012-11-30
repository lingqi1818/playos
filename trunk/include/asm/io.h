/**
 * io操作内嵌汇编命令集合
 * 移植 by ke.chenk
 * lingqi1818@gmail.com
 *
 *
 * outb和inb每次操作一个字节
 * outb_p和inb_p每次操作一个字节，但是带2个跳转延迟
 * 所以，需要加上volatile关键字去除GCC的语法优化，否则延迟语句可能被删除
 */


#define outb(value,port) \
__asm__ ("outb %%al,%%dx"::"a" (value),"d" (port))


#define inb(port) ({ \
unsigned char _v; \
__asm__ volatile ("inb %%dx,%%al":"=a" (_v):"d" (port)); \
_v; \
})

#define outb_p(value,port) \
__asm__ ("outb %%al,%%dx\n" \
		"\tjmp 1f\n" \
		"1:\tjmp 1f\n" \
		"1:"::"a" (value),"d" (port))

#define inb_p(port) ({ \
unsigned char _v; \
__asm__ volatile ("inb %%dx,%%al\n" \
	"\tjmp 1f\n" \
	"1:\tjmp 1f\n" \
	"1:":"=a" (_v):"d" (port)); \
_v; \
})
