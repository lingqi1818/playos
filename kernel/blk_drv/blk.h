#ifndef _BLK_H
#define _BLK_H
#include <sys/types.h>
#define NR_REQUEST	32

#define NR_BLK_DEV	7
struct request {
	int dev;		/* -1 if no request */
	int cmd;		/* READ or WRITE */
	int errors;
	unsigned long sector;
	unsigned long nr_sectors;
	char * buffer;
//	struct task_struct * waiting;
//	struct buffer_head * bh;
	struct request * next;
};

struct blk_dev_struct {
	void (*request_fn)(void);
	struct request * current_request;
};

extern struct blk_dev_struct blk_dev[NR_BLK_DEV];
extern struct request request[NR_REQUEST];

#ifdef MAJOR_NR
#if (MAJOR_NR == 1)
/* ram disk */
#define DEVICE_NAME "ramdisk"
#define DEVICE_REQUEST do_rd_request

#elif (MAJOR_NR == 2)
/* floppy */
#define DEVICE_NAME "floppy"
#define DEVICE_REQUEST do_fd_request

#elif (MAJOR_NR == 3)
/* harddisk */
#define DEVICE_NAME "harddisk"
#define DEVICE_INTR do_hd
#define DEVICE_REQUEST do_hd_request

#else
/* unknown blk device */
#error "unknown blk device"

#endif

#ifdef DEVICE_INTR
void (*DEVICE_INTR)(void) = NULL;
#endif

#endif

#endif
