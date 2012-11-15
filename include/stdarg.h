#ifndef _STDARG_H
#define _STDARG_H

typedef char *va_list;
//不为4的倍数的，则补齐为4的倍数
#define __va_rounded_size(TYPE)  \
  (((sizeof (TYPE) + sizeof (int) - 1) / sizeof (int)) * sizeof (int))

//每次读取一个参数，类型长度最短为4
#define va_arg(AP, TYPE)						\
 (AP += __va_rounded_size (TYPE),					\
  *((TYPE *) (AP - __va_rounded_size (TYPE))))
#endif
