/**
 * 硬盘驱动
 */
#define MAJOR_NR 3
#include "blk.h"

extern void hd_interrupt(void);

void write_char(char);
void unexpected_hd_interrupt(void)
{
	//printk("Unexpected HD interrupt\n\r");
	write_char("p");
	write_char("k");
}


void hd_init(void)
{
	blk_dev[MAJOR_NR].request_fn = DEVICE_REQUEST;
	set_intr_gate(0x2E,&hd_interrupt);//设置硬盘中断
	outb_p(inb_p(0x21)&0xfb,0x21);//允许从8259A发送中断
	outb(inb_p(0xA1)&0xbf,0xA1);//允许硬盘发送中断
}
