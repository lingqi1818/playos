#ifndef _TERMIOS_H
#define _TERMIOS_H

#define NCCS 17
//控制终端数据结构
struct termios {
	unsigned long c_iflag;		/* input mode flags */
	unsigned long c_oflag;		/* output mode flags */
	unsigned long c_lflag;		/* local mode flags */
};


/* c_iflag bits */
#define ICRNL	0000400 //回车转换成换行符

/* c_oflag bits */
#define	OPOST	0000001 //执行输出处理
#define	ONLCR	0000004 //输出时将NL转换成回车-换行符

/* c_lflag bits */
#endif
