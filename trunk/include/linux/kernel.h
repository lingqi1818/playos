#ifndef _KERNEL_H
#define _KERNEL_H
int printf(const char * fmt, ...);
int printk(const char * fmt, ...);
volatile void panic(const char * str);
#endif
