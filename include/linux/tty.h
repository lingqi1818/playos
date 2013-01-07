#ifndef _TTY_H
#define _TTY_H
#include <termios.h>

#define TTY_BUF_SIZE 1024 //队列大小

struct tty_queue {
	unsigned long data;//缓冲区中字符行数
	unsigned long head;
	unsigned long tail;
	struct task_struct * proc_list;//该队列中的阻塞进程列表
	char buf[TTY_BUF_SIZE];//队列缓冲区
};

/**
 * 注意：队列的头是不放数据的
 */
#define INC(a) ((a) = ((a)+1) & (TTY_BUF_SIZE-1))
#define DEC(a) ((a) = ((a)-1) & (TTY_BUF_SIZE-1))
#define EMPTY(a) ((a).head == (a).tail)
#define LEFT(a) (((a).tail-(a).head-1)&(TTY_BUF_SIZE-1))//队列剩余空间大小
#define LAST(a) ((a).buf[(TTY_BUF_SIZE-1)&((a).head-1)])//队列最后一个字符
#define FULL(a) (!LEFT(a))
#define CHARS(a) (((a).head-(a).tail)&(TTY_BUF_SIZE-1))
#define GETCH(queue,c) \
(void)({c=(queue).buf[(queue).tail];INC((queue).tail);})
#define PUTCH(c,queue) \
(void)({(queue).buf[(queue).head]=(c);INC((queue).head);})


struct tty_struct {
	struct termios termios;//终端IO控制数据结构
	int pgrp;//所属进程组
	int stopped;//停止标记
	void (*write)(struct tty_struct * tty);//写函数指针
	struct tty_queue read_q;//读队列
	struct tty_queue write_q;//写队列
	struct tty_queue secondary;//辅助队列，存放键盘输入 规范熟模式内容
	};

extern struct tty_struct tty_table[];
#define INIT_CC ""

void con_init(void);
void tty_init(void);

void con_write(struct tty_struct * tty);
#endif
