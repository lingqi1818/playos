/**
 * 进程调度实现
 */
#include <linux/sched.h>
#include <asm/system.h>
#include <sys/types.h>
#include <asm/io.h>
#define LATCH (1193180/HZ)

union task_union {
	struct task_struct task;
	char stack[PAGE_SIZE];
};//每个任务都定义了4K的栈空间

static union task_union init_task = {INIT_TASK,};
struct task_struct * task[NR_TASKS] = {&(init_task.task), };
//时钟中断和系统调用，定义在system_call.s中
extern int timer_interrupt(void);
extern int system_call(void);
//调度初始化
void sched_init(void) {
	int i;
	struct desc_struct * p;
	//设置init进程的tss和ldt
	set_tss_desc(gdt+FIRST_TSS_ENTRY,&(init_task.task.tss));
	set_ldt_desc(gdt+FIRST_LDT_ENTRY,&(init_task.task.ldt));
	p = gdt+2+FIRST_TSS_ENTRY;//p指向tss1
	//初始化任务的tss和idt都为0
	for(i=1;i<NR_TASKS;i++) {
			task[i] = NULL;
			p->a=p->b=0;
			p++;
			p->a=p->b=0;
			p++;
		}
	/*
	*置NT标志为0
	*嵌套任务标志NT(Nested Task)
	*嵌套任务标志NT用来控制中断返回指令IRET的执行。具体规定如下：
	*(1)、当NT=0，用堆栈中保存的值恢复EFLAGS、CS和EIP，执行常规的中断返回操作；
	*(2)、当NT=1，通过任务转换实现中断返回。
	*/
	__asm__("pushfl ; andl $0xffffbfff,(%esp) ; popfl");
	ltr(0);//加载tss0地址到tr寄存器
	lldt(0);//加载ldt0地址到ldt寄存器
	//设置8253定时器
	outb_p(0x36,0x43);//通道0，工作方式3
	//1193180为8253的工作频率，这里需要每10毫秒响应一次，
	//那么将数字设置为1193180/100
	outb_p(LATCH & 0xff , 0x40);//定时值低字节
	outb(LATCH >> 8 , 0x40);//定时值高字节
	set_intr_gate(0x20,&timer_interrupt);//设置时钟中断门
	outb(inb_p(0x21)&~0x01,0x21);//允许时钟中断
	set_system_gate(0x80,&system_call);//设置系统调用中断门
}
