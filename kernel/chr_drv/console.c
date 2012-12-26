#include <linux/sched.h>
#include <linux/tty.h>
#include <asm/io.h>
#include <asm/system.h>
/**
 * 终端初始化&终端输出的实现
 * lingqi1818@gmail.com
 */

#define ORIG_X			(*(unsigned char *)0x90000)//初始光标列号
#define ORIG_Y			(*(unsigned char *)0x90001)//初始光标行号
#define ORIG_VIDEO_PAGE		(*(unsigned short *)0x90004)//显示页面
#define ORIG_VIDEO_MODE		((*(unsigned short *)0x90006) & 0xff)//显示模式
#define ORIG_VIDEO_COLS 	(((*(unsigned short *)0x90006) & 0xff00) >> 8)//字符列数
#define ORIG_VIDEO_LINES	(25)//显示行数
#define ORIG_VIDEO_EGA_AX	(*(unsigned short *)0x90008)//[??]
#define ORIG_VIDEO_EGA_BX	(*(unsigned short *)0x9000a)//显示内存大小和色彩模式
#define ORIG_VIDEO_EGA_CX	(*(unsigned short *)0x9000c)//显示卡特性参数

#define VIDEO_TYPE_MDA		0x10	/* Monochrome Text Display	单色文本*/
#define VIDEO_TYPE_CGA		0x11	/* CGA Display 			*/
#define VIDEO_TYPE_EGAM		0x20	/* EGA/VGA in Monochrome Mode	单色*/
#define VIDEO_TYPE_EGAC		0x21	/* EGA/VGA in Color Mode	彩色*/

#define NPAR 16

extern void keyboard_interrupt(void);

static unsigned char	video_type;		/* Type of display being used	显示类型*/
static unsigned long	video_num_columns;	/* Number of text columns	文本列数*/
static unsigned long	video_size_row;		/* Bytes per row		每行字节数*/
static unsigned long	video_num_lines;	/* Number of test lines		文本行数*/
static unsigned char	video_page;		/* Initial video page		初始显示页面*/
static unsigned long	video_mem_start;	/* Start of video RAM		显存起始地址*/
static unsigned long	video_mem_end;		/* End of video RAM (sort of)	显存结束地址*/
static unsigned short	video_port_reg;		/* Video register select port	显示控制索引寄存器端口*/
static unsigned short	video_port_val;		/* Video register value port	显示控制数据寄存器端口*/
static unsigned short	video_erase_char;	/* Char+Attrib to erase with	擦出字符属性及字符*/

static unsigned long	origin;		/* Used for EGA/VGA fast scroll	滚屏其实内存地址*/
static unsigned long	scr_end;	/* Used for EGA/VGA fast scroll	滚屏末端内存地址*/
static unsigned long	pos;//当前光标对应显存地址
static unsigned long	x,y;//当前光标位置
static unsigned long	top,bottom;//滚动时顶行行号和底行行号
static unsigned long	state=0;//ANSI转义字符序列处理状态
static unsigned long	npar,par[NPAR];//ANSI转义字符序列参数个数和参数数组
static unsigned long	ques=0;//收到问号字符标志
static unsigned char	attr=0x07;//字符属性，黑底白字


static inline void gotoxy(unsigned int new_x,unsigned int new_y)
{
	if (new_x > video_num_columns || new_y >= video_num_lines)
		return;
	x=new_x;
	y=new_y;
	pos=origin + y*video_size_row + (x<<1);
}

void con_init(void) {
	register unsigned char a;
	char *display_desc = "????";
	char *display_ptr;

	video_num_columns = ORIG_VIDEO_COLS;
	video_size_row = video_num_columns * 2;
	video_num_lines = ORIG_VIDEO_LINES;
	video_page = ORIG_VIDEO_PAGE;
	video_erase_char = 0x0720;//擦除字符0x20,0x07为属性

	if (ORIG_VIDEO_MODE == 7)			/* Is this a monochrome display? 是否单色模式*/
		{
			video_mem_start = 0xb0000;
			video_port_reg = 0x3b4;
			video_port_val = 0x3b5;
			if ((ORIG_VIDEO_EGA_BX & 0xff) != 0x10)
			{
				video_type = VIDEO_TYPE_EGAM;
				video_mem_end = 0xb8000;
				display_desc = "EGAm";
			}
			else
			{
				video_type = VIDEO_TYPE_MDA;
				video_mem_end	= 0xb2000;
				display_desc = "*MDA";
			}
		}
		else								/* If not, it is color. 彩色模式*/
		{
			video_mem_start = 0xb8000;
			video_port_reg	= 0x3d4;
			video_port_val	= 0x3d5;
			if ((ORIG_VIDEO_EGA_BX & 0xff) != 0x10)
			{
				video_type = VIDEO_TYPE_EGAC;
				video_mem_end = 0xbc000;
				display_desc = "EGAc";
			}
			else
			{
				video_type = VIDEO_TYPE_CGA;
				video_mem_end = 0xba000;
				display_desc = "*CGA";
			}
		}
	//指针指向第一行倒数差4字节处
	display_ptr = ((char *)video_mem_start) + video_size_row - 8;
	//复制display_desc内容到display_ptr处
	while (*display_desc)
		{
			*display_ptr++ = *display_desc++;
			display_ptr++;
		}

	//初始化用于滚屏的变量
	origin	= video_mem_start;
	scr_end	= video_mem_start + video_num_lines * video_size_row;
	top	= 0;
	bottom	= video_num_lines;

	gotoxy(ORIG_X,ORIG_Y);
	set_trap_gate(0x21,&keyboard_interrupt);
	outb_p(inb_p(0x21)&0xfd,0x21);//允许键盘中断
	a=inb_p(0x61);
	outb_p(a|0x80,0x61);//禁用键盘
	outb(a,0x61);//开启键盘（重置）
}
