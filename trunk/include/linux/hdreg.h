#ifndef _HDREG_H
#define _HDREG_H

//硬盘控制器寄存器端口
#define HD_DATA		0x1f0	/* _CTL when writing */
#define HD_ERROR	0x1f1	/* see err-bits */
#define HD_NSECTOR	0x1f2	/* nr of sectors to read/write */
#define HD_SECTOR	0x1f3	/* starting sector */
#define HD_LCYL		0x1f4	/* starting cylinder */
#define HD_HCYL		0x1f5	/* high byte of starting cyl */
#define HD_CURRENT	0x1f6	/* 101dhhhh , d=drive, hhhh=head */
#define HD_STATUS	0x1f7	/* see status-bits */
#define HD_PRECOMP HD_ERROR	/* same io address, read=error, write=precomp */
#define HD_COMMAND HD_STATUS

//控制寄存器端口
#define HD_CMD		0x3f6
/*
 * 硬盘状态寄存器各位定义
 */
#define ERR_STAT	0x01 //命令执行错误
#define INDEX_STAT	0x02 //收到索引
#define ECC_STAT	0x04 //ECC校验错误
#define DRQ_STAT	0x08 //请求服务
#define SEEK_STAT	0x10 //寻道结束
#define WRERR_STAT	0x20 //驱动器故障
#define READY_STAT	0x40 //驱动器就绪
#define BUSY_STAT	0x80 //控制器忙碌
/**
 * 硬盘命令集合
 */
#define WIN_RESTORE		0x10 //驱动器复位
#define WIN_READ		0x20 //读扇区
#define WIN_WRITE		0x30 //写扇区
#define WIN_VERIFY		0x40 //扇区检查
#define WIN_FORMAT		0x50 //格式化磁道
#define WIN_INIT		0x60 //控制器初始化
#define WIN_SEEK 		0x70 //寻道
#define WIN_DIAGNOSE		0x90 //控制器诊断
#define WIN_SPECIFY		0x91 //建立驱动器参数

/**
 * 硬盘分区表
 */
struct partition {
	unsigned char boot_ind;		//引导标志，4个分区只有1个是可以引导的。0x0不从该分区引导操作系统，0x80从该分区引导
	unsigned char head;		//分区起始磁头号
	unsigned char sector;		//分区起始扇区号（位0-5），起始柱面高2位（位6-7）
	unsigned char cyl;		//起始柱面低8位
	unsigned char sys_ind;		//分区类型：0x80:minix
	unsigned char end_head;		//分区结束磁头号
	unsigned char end_sector;	//分区结束扇区号（位0-5），结束柱面高2位（位6-7）
	unsigned char end_cyl;		//结束柱面低8位
	unsigned int start_sect;	//分区起始物理扇区号
	unsigned int nr_sects;		//分区占用扇区数
};

#endif
