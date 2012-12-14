/**
 * 描述符，门等通用系统设置
 */
//从权限级0进入到权限级3，开始任务0
#define move_to_user_mode() \
/*保存堆栈指针到eax*/__asm__ ("movl %%esp,%%eax\n\t" \
/*堆栈段选择符入栈*/	"pushl $0x17\n\t" \
/*esp入栈*/			"pushl %%eax\n\t" \
/*标志寄存器入栈*/		"pushfl\n\t" \
/*task0代码段cs入栈*/	"pushl $0x0f\n\t" \
/*task0 eip*/		"pushl $1f\n\t" \
/*中断返回跳转到1处，任务0开始运行*/	"iret\n" \
/*后面为任务0初始化操作*/	"1:\tmovl $0x17,%%eax\n\t" \
	"movw %%ax,%%ds\n\t" \
	"movw %%ax,%%es\n\t" \
	"movw %%ax,%%fs\n\t" \
	"movw %%ax,%%gs" \
	:::"ax")
#define sti() __asm__ ("sti"::)
#define cli() __asm__ ("cli"::)
#define nop() __asm__ ("nop"::)

#define iret() __asm__ ("iret"::)
#define _set_gate(gate_addr,type,dpl,addr) \
__asm__ ("movw %%dx,%%ax\n\t" \
	"movw %0,%%dx\n\t" \
	"movl %%eax,%1\n\t" \
	"movl %%edx,%2" \
	: \
	: "i" ((short) (0x8000+(dpl<<13)+(type<<8))), \
	"o" (*((char *) (gate_addr))), \
	"o" (*(4+(char *) (gate_addr))), \
	"d" ((char *) (addr)),"a" (0x00080000))


#define set_intr_gate(n,addr) \
	_set_gate(&idt[n],14,0,addr)

#define set_trap_gate(n,addr) \
	_set_gate(&idt[n],15,0,addr)

#define set_system_gate(n,addr) \
	_set_gate(&idt[n],15,3,addr)

/**
 * 在gdt中设置tss,ldt描述符
 * n:gdt中表项n对应的地址
 * addr:tss/ldt内存基地址
 * type:gdt描述符中的type字段
 */
#define _set_tssldt_desc(n,addr,type) \
/*tss和ldt的长度都被设置成了104字节*/__asm__ ("movw $104,%1\n\t" \
/*基地址的低16位写入到描述符偏移2字节处*/	"movw %%ax,%2\n\t" \
/*基地址高16位循环右移到低16位处*/	"rorl $16,%%eax\n\t" \
/*高地址低8位写入到描述符偏移4字节处*/	"movb %%al,%3\n\t" \
/*type字段*/	"movb $" type ",%4\n\t" \
	"movb $0x00,%5\n\t" \
/*高地址高8位写入到描述符高8位处*/	"movb %%ah,%6\n\t" \
/*循环右移，恢复eax*/	"rorl $16,%%eax" \
	::"a" (addr), "m" (*(n)), "m" (*(n+2)), "m" (*(n+4)), \
	 "m" (*(n+5)), "m" (*(n+6)), "m" (*(n+7)) \
	)

#define set_tss_desc(n,addr) _set_tssldt_desc(((char *) (n)),addr,"0x89")
#define set_ldt_desc(n,addr) _set_tssldt_desc(((char *) (n)),addr,"0x82")
