#ifndef _HDREG_H
#define _HDREG_H

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

#endif
