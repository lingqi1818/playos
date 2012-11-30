/**
 * gdt,ldt表定义于此
 */

#ifndef _HEAD_H
#define _HEAD_H

typedef struct desc_struct {
	unsigned long a,b;
} desc_table[256];//大小为256项的描述符表，每项占8个字节

//注意：idt,gdt,pg_dir的地址都是在head.s中定义的
extern unsigned long pg_dir[1024];
extern desc_table idt,gdt;//gdt,idt定义在head.s中
#endif
