#ifndef _KERNEL_H
#define _KERNEL_H
int printk(const char * fmt, ...);
volatile void panic(const char * str);
#endif
