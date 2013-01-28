/**
 * 向底层块设备发送读写请求
 */
#include "blk.h"
#include <linux/fs.h>
#include <asm/system.h>

struct request request[NR_REQUEST];//硬盘操作请求数组
struct task_struct * wait_for_request = NULL;//请求数组没有空闲项的等待队列

struct blk_dev_struct blk_dev[NR_BLK_DEV] = {
	{ NULL, NULL },		/* no_dev */
	{ NULL, NULL },		/* dev mem */
	{ NULL, NULL },		/* dev fd */
	{ NULL, NULL },		/* dev hd */
	{ NULL, NULL },		/* dev ttyx */
	{ NULL, NULL },		/* dev tty */
	{ NULL, NULL }		/* dev lp */
};

static inline void lock_buffer(struct buffer_head * bh)
{
	cli();
	while (bh->b_lock)
		sleep_on(&bh->b_wait);
	bh->b_lock=1;
	sti();
}

static inline void unlock_buffer(struct buffer_head * bh)
{
	if (!bh->b_lock)
		printk("ll_rw_block.c: buffer not locked\n\r");
	bh->b_lock = 0;
	wake_up(&bh->b_wait);
}

//添加请求到队列
static void add_request(struct blk_dev_struct * dev, struct request * req)
{
	struct request * tmp;

	req->next = NULL;
	cli();
	if (req->bh)
		req->bh->b_dirt = 0;
	if (!(tmp = dev->current_request)) {
		dev->current_request = req;//如果设备当前请求为空，则直接执行request_fn
		sti();//不开中断硬盘中断函数无法执行
		(dev->request_fn)();
		return;
	}
	for ( ; tmp->next ; tmp=tmp->next)//否则执行电梯算法，将请求插入队列，读优先级高于写
		if ((IN_ORDER(tmp,req) ||
		    !IN_ORDER(tmp,tmp->next)) &&
		    IN_ORDER(req,tmp->next))
			break;
	req->next=tmp->next;
	tmp->next=req;
	sti();
}

/**
 * 创建读写请求
 */
static void make_request(int major,int rw, struct buffer_head * bh)
{
	struct request * req;
	int rw_ahead;

	/**
	 * 预读和预写被当做普通读写来处理
	 */
	if (rw_ahead = (rw == READA || rw == WRITEA)) {
		if (bh->b_lock)//当缓存块被锁住，预读写的情况则直接退出
			return;
		if (rw == READA)
			rw = READ;
		else
			rw = WRITE;
	}
	if (rw!=READ && rw!=WRITE)
		panic("Bad block dev command, must be R/W/RA/WA");
	lock_buffer(bh);//锁住缓存块
	if ((rw == WRITE && !bh->b_dirt) || (rw == READ && bh->b_uptodate)) {//如果写的时候缓存不脏，读的时候缓存已经被更新过，那么直接返回
		unlock_buffer(bh);//解锁缓冲块
		return;
	}
repeat:

	if (rw == READ)
		req = request+NR_REQUEST;//从请求队列尾部开始读
	else
		req = request+((NR_REQUEST*2)/3);//从请求队列2/3处开始写
	//查找请求数组的空闲项
	while (--req >= request)
		if (req->dev<0)
			break;
	//没有找到空闲项
	if (req < request) {
		if (rw_ahead) {//如果是预读写，则直接返回
			unlock_buffer(bh);
			return;
		}
		sleep_on(&wait_for_request);//否则在request队列上sleep
		goto repeat;
	}

	req->dev = bh->b_dev;
	req->cmd = rw;
	req->errors=0;
	req->sector = bh->b_blocknr<<1;
	req->nr_sectors = 2;
	req->buffer = bh->b_data;
	req->waiting = NULL;
	req->bh = bh;
	req->next = NULL;
	add_request(major+blk_dev,req);
}

/*
 * 将指定的缓存块添加到块设备读写队列中
 */
void ll_rw_block(int rw, struct buffer_head * bh)
{
	unsigned int major;
	//校验参数
	if ((major=MAJOR(bh->b_dev)) >= NR_BLK_DEV ||
	!(blk_dev[major].request_fn)) {
		printk("Trying to read nonexistent block-device\n\r");
		return;
	}
	make_request(major,rw,bh);
}

void blk_dev_init(void)
{
	int i;

	for (i=0 ; i<NR_REQUEST ; i++) {
		request[i].dev = -1;
		request[i].next = NULL;
	}
}
