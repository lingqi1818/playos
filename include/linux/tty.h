#ifndef _TTY_H
#define _TTY_H
#include <termios.h>

#define TTY_BUF_SIZE 1024 //队列大小

struct tty_queue {
	unsigned long data;//缓冲区中字符行数
	unsigned long head;
	unsigned long tail;
	//struct task_struct * proc_list;
	char buf[TTY_BUF_SIZE];//队列缓冲区
};

struct tty_struct {
	struct termios termios;//终端IO控制数据结构
	int pgrp;//所属进程组
	int stopped;//停止标记
	void (*write)(struct tty_struct * tty);//写函数指针
	struct tty_queue read_q;//读队列
	struct tty_queue write_q;//写队列
	//struct tty_queue secondary;
	};

extern struct tty_struct tty_table[];
#define INIT_CC ""

#endif
