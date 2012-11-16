#ifndef _STDARG_H
#define _STDARG_H

typedef char *va_list;
//不为4的倍数的，则补齐为4的倍数(原因是32位系统的堆栈是按照4个字节来对齐的)
#define __va_rounded_size(TYPE)  \
  (((sizeof (TYPE) + sizeof (int) - 1) / sizeof (int)) * sizeof (int))


#define va_start(AP, LASTARG)  (AP = ((char *) &(LASTARG) + __va_rounded_size (LASTARG)))

void va_end (va_list);
#define va_end(AP)//暂时没有va_end的实现，所以先把该函数的调用都注释掉。

//每次读取一个参数，类型长度最短为4
#define va_arg(AP, TYPE)						\
 (AP += __va_rounded_size (TYPE),					\
  *((TYPE *) (AP - __va_rounded_size (TYPE))))
#endif
