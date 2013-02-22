/*
*文件系统头文件
*
*mail:lingqi1818@gmail.com
*/

//设备号=主设备号*256+次设备号
//主设备号：1-内存，2-磁盘，3-硬盘，4-ttyx,5-tty,6-并行口，7-非命名管道
#ifndef _FS_H
#define _FS_H
#include <sys/types.h>
#define READ 0
#define WRITE 1
#define READA 2		/* read-ahead - don't pause */
#define WRITEA 3	/* "write-ahead" - silly, but somewhat useful */

#define MAJOR(a) (((unsigned)(a))>>8)
#define MINOR(a) ((a)&0xff) 
extern int ROOT_DEV;

#define NR_HASH 307
#define NR_FILE 64

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

//文件结构
struct file {
	unsigned short f_mode;//rw位
	unsigned short f_flags;//文件打开和控制标识
	unsigned short f_count;//引用数
	struct m_inode * f_inode;//对应i节点
	off_t f_pos;//文件读写偏移值
};

/**
 * 磁盘超级块
 */
struct super_block {
	unsigned short s_ninodes;//节点数
	unsigned short s_nzones;//逻辑块数
	unsigned short s_imap_blocks;//i节点位图所占数据块数
	unsigned short s_zmap_blocks;//逻辑块位图所占数据块数
	unsigned short s_firstdatazone;//第一个数据逻辑块号
	unsigned short s_log_zone_size;//log(数据块/逻辑块)
	unsigned long s_max_size;//文件最大长度
	unsigned short s_magic;//文件系统魔数
/* These are only in memory */
	struct buffer_head * s_imap[8];//i节点位图缓冲块指针数组
	struct buffer_head * s_zmap[8];//逻辑块位图缓冲块指针数组
	unsigned short s_dev;//超级块所在设备号
	struct m_inode * s_isup;//被安装文件系统根目录的i节点
	struct m_inode * s_imount;//被安装到的i节点
	unsigned long s_time;//修改时间
	struct task_struct * s_wait;//等待该超级块进程
	unsigned char s_lock;//锁定标志
	unsigned char s_rd_only;//只读标志
	unsigned char s_dirt;//已修改（脏）标志
};

/**
 * 磁盘上的i节点(文件索引节点)数据结构，代表一个文件
 */
struct d_inode {
	unsigned short i_mode;//rwx属性
	unsigned short i_uid;//用户id
	unsigned long i_size;//文件大小
	unsigned long i_time;//修改时间
	unsigned char i_gid;//组ID
	unsigned char i_nlinks;//链接数（多少个文件目录项指向该i节点）
	unsigned short i_zone[9];//区段，逻辑块。直接0-6，间接7，双重间接8
};

struct m_inode {
	unsigned short i_mode;
	unsigned short i_uid;
	unsigned long i_size;
	unsigned long i_mtime;
	unsigned char i_gid;
	unsigned char i_nlinks;
	unsigned short i_zone[9];
/* these are in memory also */
	struct task_struct * i_wait;//等待该i节点进程
	unsigned long i_atime;//最后访问时间
	unsigned long i_ctime;//修改时间
	unsigned short i_dev;//所在设备号
	unsigned short i_num;//i节点号
	unsigned short i_count;//i节点使用次数
	unsigned char i_lock;//锁定
	unsigned char i_dirt;//脏
	unsigned char i_pipe;//管道标志
	unsigned char i_mount;//安装标志
	unsigned char i_seek;//搜寻标识（lseek时）
	unsigned char i_update;//更新标识
};


extern struct file file_table[NR_FILE];
#endif
