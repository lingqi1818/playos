/*
*文件系统头文件
*
*mail:lingqi1818@gmail.com
*/

//设备号=主设备号*256+次设备号
//主设备号：1-内存，2-磁盘，3-硬盘，4-ttyx,5-tty,6-并行口，7-非命名管道
#ifndef _FS_H
#define _FS_H

#define READ 0
#define WRITE 1
#define READA 2		/* read-ahead - don't pause */
#define WRITEA 3	/* "write-ahead" - silly, but somewhat useful */

#define MAJOR(a) (((unsigned)(a))>>8)
#define MINOR(a) ((a)&0xff) 
extern int ROOT_DEV;

#define NR_HASH 307

#define NR_BUFFERS nr_buffers
#define BLOCK_SIZE 1024

/**
 * 缓冲区块数据结构
 */
struct buffer_head {
	char * b_data;			/* 数据指针 */
	unsigned long b_blocknr;	/* 块号 */
	unsigned short b_dev;		/* 设备号 */
	unsigned char b_uptodate;	/*是否更新过，即和硬盘数据一致*/
	unsigned char b_dirt;		/* 0-clean,1-dirty */
	unsigned char b_count;		/* 用户使用数量 */
	unsigned char b_lock;		/* 0 - ok, 1 -locked */
	struct task_struct * b_wait; /*在缓冲区块上等待的进程链表*/
	struct buffer_head * b_prev; /*同一个hash队列上的上一块*/
	struct buffer_head * b_next; /*同一个hash队列上的下一块*/
	struct buffer_head * b_prev_free;/*缓冲区空闲表上前一块*/
	struct buffer_head * b_next_free;/*缓冲区空闲表上下一块*/
};
#endif
