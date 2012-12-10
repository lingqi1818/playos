/**
 * 调度系统头文件
 * ke.chenk
 * lingqi1818@gmail.com
 */

#ifndef _SCHED_H
#define _SCHED_H

#include <linux/head.h>
#include <linux/mm.h>

#define TASK_RUNNING		0
#define TASK_INTERRUPTIBLE	1
#define TASK_UNINTERRUPTIBLE	2
#define TASK_ZOMBIE		3
#define TASK_STOPPED		4
#define NR_TASKS 64 //默认任务数
#define HZ 100

#define FIRST_TASK task[0]
#define LAST_TASK task[NR_TASKS-1]

/*
 * Entry into gdt where to find first TSS. 0-nul, 1-cs, 2-ds, 3-syscall
 * 4-TSS0, 5-LDT0, 6-TSS1 etc ...
 */
extern void sched_init(void);

#define FIRST_TSS_ENTRY 4
#define FIRST_LDT_ENTRY (FIRST_TSS_ENTRY+1)
//每个任务占用1个TSS和1个LDT，一共占16个字节
#define _TSS(n) ((((unsigned long) n)<<4)+(FIRST_TSS_ENTRY<<3))//计算TSS在GDT中的起始位置
#define _LDT(n) ((((unsigned long) n)<<4)+(FIRST_LDT_ENTRY<<3))//计算ldt在GDT中的起始位置
#define ltr(n) __asm__("ltr %%ax"::"a" (_TSS(n)))
#define lldt(n) __asm__("lldt %%ax"::"a" (_LDT(n)))


#define switch_to(n) {\
struct {long a,b;} __tmp; \
__asm__("cmpl %%ecx,_current\n\t" \
	"je 1f\n\t" \
	"movw %%dx,%1\n\t" \
	"xchgl %%ecx,_current\n\t" \
	"ljmp %0\n\t" \
	"cmpl %%ecx,_last_task_used_math\n\t" \
	"jne 1f\n\t" \
	"clts\n" \
	"1:" \
	::"m" (*&__tmp.a),"m" (*&__tmp.b), \
	"d" (_TSS(n)),"c" ((long) task[n])); \
}


struct tss_struct {
	long back_link; /* 16 high bits zero */
	long esp0;
	long ss0; /* 16 high bits zero */
	long esp1;
	long ss1; /* 16 high bits zero */
	long esp2;
	long ss2; /* 16 high bits zero */
	long cr3;
	long eip;
	long eflags;
	long eax, ecx, edx, ebx;
	long esp;
	long ebp;
	long esi;
	long edi;
	long es; /* 16 high bits zero */
	long cs; /* 16 high bits zero */
	long ss; /* 16 high bits zero */
	long ds; /* 16 high bits zero */
	long fs; /* 16 high bits zero */
	long gs; /* 16 high bits zero */
	long ldt; /* 16 high bits zero */
	long trace_bitmap; /* bits: trace 0, bitmap 16-31 */
};

struct task_struct {
	long state;	/* -1 unrunnable, 0 runnable, >0 stopped */
	long counter;
	long priority;
	long signal;
	struct sigaction sigaction[32];
	long blocked;	/* bitmap of masked signals */
	long alarm;
	long utime,stime,cutime,cstime,start_time;
	/* ldt for this task 0 - zero 1 - cs 2 - ds&ss */
	struct desc_struct ldt[3];
	/* tss for this task */
	struct tss_struct tss;
};


#define INIT_TASK \
		{ \
			{ \
			{0,0}, \
/* ldt */	{0x9f,0xc0fa00}, \
			{0x9f,0xc0f200}, \
			}, \
			{0,PAGE_SIZE+(long)&init_task,0x10,0,0,0,0,(long)&pg_dir,\
			0,0,0,0,0,0,0,0, \
			0,0,0x17,0x17,0x17,0x17,0x17,0x17, \
			_LDT(0),0x80000000 \
			} \
		}
#endif
