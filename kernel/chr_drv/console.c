#include <linux/sched.h>
#include <linux/tty.h>
#include <asm/io.h>
#include <asm/system.h>
/**
 * 终端初始化&终端输出的实现
 *
 * 注意：屏幕有两种卷动方式：满屏卷动&部分卷动（top~bottom的数据移动）
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

static unsigned char video_type; /* Type of display being used	显示类型*/
static unsigned long video_num_columns; /* Number of text columns	文本列数*/
static unsigned long video_size_row; /* Bytes per row		每行字节数*/
static unsigned long video_num_lines; /* Number of test lines		文本行数*/
static unsigned char video_page; /* Initial video page		初始显示页面*/
static unsigned long video_mem_start; /* Start of video RAM		显存起始地址*/
static unsigned long video_mem_end; /* End of video RAM (sort of)	显存结束地址*/
static unsigned short video_port_reg; /* Video register select port	显示控制索引寄存器端口*/
static unsigned short video_port_val; /* Video register value port	显示控制数据寄存器端口*/
static unsigned short video_erase_char; /* Char+Attrib to erase with	擦出字符属性及字符*/

static unsigned long origin; /* Used for EGA/VGA fast scroll	滚屏起始内存地址（新屏起始位置）*/
static unsigned long scr_end; /* Used for EGA/VGA fast scroll	滚屏末端内存地址*/
static unsigned long pos; //当前光标对应显存地址
static unsigned long x, y; //当前光标位置
static unsigned long top, bottom; //滚动时顶行行号和底行行号
static unsigned long state = 0; //ANSI转义字符序列处理状态
static unsigned long npar, par[NPAR]; //ANSI转义字符序列参数个数和参数数组
static unsigned long ques = 0; //收到问号字符标志
static unsigned char attr = 0x07; //字符属性，黑底白字

static inline void gotoxy(unsigned int new_x, unsigned int new_y) {
	if (new_x > video_num_columns || new_y >= video_num_lines)
		return;
	x = new_x;
	y = new_y;
	pos = origin + y * video_size_row + (x << 1);
}

static inline void set_origin(void)
{
	cli();
	outb_p(12, video_port_reg);
	outb_p(0xff&((origin-video_mem_start)>>9), video_port_val);//origin高位，右移8位为获取高位，再右移1位是因为每个字符在内存中占用一个字的长度
	outb_p(13, video_port_reg);
	outb_p(0xff&((origin-video_mem_start)>>1), video_port_val);//origin低位
	sti();
}

/**
 * 从top处插入一行，光标后的内容向下滚动一行。
 * 这里if和else的代码相同。
 */
static void scrdown(void)
{
	if (video_type == VIDEO_TYPE_EGAC || video_type == VIDEO_TYPE_EGAM)
	{
		__asm__("std\n\t"
			"rep\n\t"
			"movsl\n\t"
			"addl $2,%%edi\n\t"	/* %edi has been decremented by 4 */
			"movl _video_num_columns,%%ecx\n\t"
			"rep\n\t"
			"stosw"
			::"a" (video_erase_char),
			"c" ((bottom-top-1)*video_num_columns>>1),
			"D" (origin+video_size_row*bottom-4),//由于方向位置反，所以movsl是从每行最后一个双字开始复制的。
			"S" (origin+video_size_row*(bottom-1)-4)
			:"ax","cx","di","si");
	}
	else		/* Not EGA/VGA */
	{
		__asm__("std\n\t"
			"rep\n\t"
			"movsl\n\t"
			"addl $2,%%edi\n\t"	/* %edi has been decremented by 4 */
			"movl _video_num_columns,%%ecx\n\t"
			"rep\n\t"
			"stosw"
			::"a" (video_erase_char),
			"c" ((bottom-top-1)*video_num_columns>>1),
			"D" (origin+video_size_row*bottom-4),
			"S" (origin+video_size_row*(bottom-1)-4)
			:"ax","cx","di","si");
	}
}

//向上卷动一行
static void scrup(void)
{
	if (video_type == VIDEO_TYPE_EGAC || video_type == VIDEO_TYPE_EGAM)
	{
		if (!top && bottom == video_num_lines) {
			origin += video_size_row;
			pos += video_size_row;
			scr_end += video_size_row;
			if (scr_end > video_mem_end) {
				__asm__("cld\n\t"
					"rep\n\t"
					"movsl\n\t"
					"movl _video_num_columns,%1\n\t"//当前行冲入空值，cx为0，所以需要重新设置
					"rep\n\t"
					"stosw"
					::"a" (video_erase_char),
					"c" ((video_num_lines-1)*video_num_columns>>1),
					"D" (video_mem_start),
					"S" (origin)
					:"cx","di","si");
				scr_end -= origin-video_mem_start;
				pos -= origin-video_mem_start;
				origin = video_mem_start;
			} else {
				__asm__("cld\n\t"
					"rep\n\t"
					"stosw"
					::"a" (video_erase_char),
					"c" (video_num_columns),
					"D" (scr_end-video_size_row)
					:"cx","di");
			}
			set_origin();
		} else {
			__asm__("cld\n\t"
				"rep\n\t"
				"movsl\n\t"
				"movl _video_num_columns,%%ecx\n\t"
				"rep\n\t"
				"stosw"
				::"a" (video_erase_char),
				"c" ((bottom-top-1)*video_num_columns>>1),
				"D" (origin+video_size_row*top),
				"S" (origin+video_size_row*(top+1))
				:"cx","di","si");
		}
	}
	else		/* Not EGA/VGA  MDA模式每次都需要手工卷屏，不能做到当前屏自动卷动*/
	{
		__asm__("cld\n\t"
			"rep\n\t"
			"movsl\n\t"
			"movl _video_num_columns,%%ecx\n\t"
			"rep\n\t"
			"stosw"
			::"a" (video_erase_char),
			"c" ((bottom-top-1)*video_num_columns>>1),
			"D" (origin+video_size_row*top),
			"S" (origin+video_size_row*(top+1))
			:"cx","di","si");
	}
}

static void cr(void)
{
	pos -= x<<1;
	x=0;
}

static void lf(void)
{
	if (y+1<bottom) {
		y++;
		pos += video_size_row;
		return;
	}
	scrup();
}

/**
 * 删除屏幕中和光标相关的内容
 * 转移序列为：'ESC [ Ps J'
 * Ps=0:删除光标到屏幕底端的内容
 * Ps=1:删除屏幕开头到光标处的内容
 * Ps=2:删除整屏内容
 */
static void csi_J(int par)
{
	long count __asm__("cx");
	long start __asm__("di");

	switch (par) {
		case 0:	/* erase from cursor to end of display */
			count = (scr_end-pos)>>1;
			start = pos;
			break;
		case 1:	/* erase from start to cursor */
			count = (pos-origin)>>1;
			start = origin;
			break;
		case 2: /* erase whole display */
			count = video_num_columns * video_num_lines;
			start = origin;
			break;
		default:
			return;
	}
	__asm__("cld\n\t"
		"rep\n\t"
		"stosw\n\t"
		::"c" (count),
		"D" (start),"a" (video_erase_char)
		:"cx","di");
}

/**
 * 删除一行中和光标相关的内容
 * 转移序列为：'ESC [ Ps K'
 * Ps=0:删除光标到行末的内容
 * Ps=1:删除行开头到光标处的内容
 * Ps=2:删除整行内容
 */
static void csi_K(int par)
{
	long count __asm__("cx");
	long start __asm__("di");

	switch (par) {
		case 0:	/* erase from cursor to end of line */
			if (x>=video_num_columns)
				return;
			count = video_num_columns-x;
			start = pos;
			break;
		case 1:	/* erase from start of line to cursor */
			start = pos - (x<<1);
			count = (x<video_num_columns)?x:video_num_columns;
			break;
		case 2: /* erase whole line */
			start = pos - (x<<1);
			count = video_num_columns;
			break;
		default:
			return;
	}
	__asm__("cld\n\t"
		"rep\n\t"
		"stosw\n\t"
		::"c" (count),
		"D" (start),"a" (video_erase_char)
		:"cx","di");
}

/**
 * 在光标处插入指定的行数,光标以下内容往下滚
 */
static void insert_line(void)
{
	int oldtop,oldbottom;

	oldtop=top;
	oldbottom=bottom;
	top=y;
	bottom = video_num_lines;
	scrdown();
	top=oldtop;
	bottom=oldbottom;
}


static void delete_line(void)
{
	int oldtop,oldbottom;

	oldtop=top;
	oldbottom=bottom;
	top=y;
	bottom = video_num_lines;
	scrup();
	top=oldtop;
	bottom=oldbottom;
}

/**
 * 在光标处插入指定的行数,光标以下内容往下滚
 */
static void csi_L(unsigned int nr)
{
	if (nr > video_num_lines)
		nr = video_num_lines;
	else if (!nr)
		nr = 1;
	while (nr--)
		insert_line();
}

/**
 * 删除光标开始处的N行，下面的内容往上滚
 */
static void csi_M(unsigned int nr)
{
	if (nr > video_num_lines)
		nr = video_num_lines;
	else if (!nr)
		nr=1;
	while (nr--)
		delete_line();
}

/**
 * 删除光标处的一个字符，光标右侧的字符都左移一格
 */
static void delete_char(void)
{
	int i;
	unsigned short * p = (unsigned short *) pos;

	if (x>=video_num_columns)
		return;
	i = x;
	while (++i < video_num_columns) {
		*p = *(p+1);
		p++;
	}
	*p = video_erase_char;
}

/**
 * 删除光标处n个字符
 */
static void csi_P(unsigned int nr)
{
	if (nr > video_num_columns)
		nr = video_num_columns;
	else if (!nr)
		nr = 1;
	while (nr--)
		delete_char();
}

/**
 * 在光标处插入一个空格
 */
static void insert_char(void)
{
	int i=x;
	unsigned short tmp, old = video_erase_char;
	unsigned short * p = (unsigned short *) pos;

	while (i++<video_num_columns) {
		tmp=*p;
		*p=old;
		old=tmp;
		p++;
	}
}

/**
 * 在光标处插入n个字符
 */
static void csi_at(unsigned int nr)
{
	if (nr > video_num_columns)
		nr = video_num_columns;
	else if (!nr)
		nr = 1;
	while (nr--)
		insert_char();
}

/**
 * 设置显示字符属性
 * 0：默认属性
 * 1：加粗
 * 4：下划线
 * 7：反显
 * 27：正显
 * 这仅仅是个简化处理的函数，其实更复杂。
 */
void csi_m(void)
{
	int i;

	for (i=0;i<=npar;i++)
		switch (par[i]) {
			case 0:attr=0x07;break;
			case 1:attr=0x0f;break;
			case 4:attr=0x0f;break;
			case 7:attr=0x70;break;
			case 27:attr=0x07;break;
		}
}

static int saved_x=0;
static int saved_y=0;

static void save_cur(void)
{
	saved_x=x;
	saved_y=y;
}

static void restore_cur(void)
{
	gotoxy(saved_x, saved_y);
}

//输出到当前tty的控制台
void con_write(struct tty_struct * tty) {
	int nr; //缓冲队列当前字符数
	char c; //缓冲队列指针当前字符
	nr = CHARS(tty->write_q);
	while (nr--) {
		GETCH(tty->write_q, c);
		//以下为处理字符c的逻辑
		//state=0，如果c>31&&c<127则正常输出。
		//state=1，则处理转移序列，state=2,3,4则是处理过程的几个步骤。
		switch(state) {

		case 0:
			if (c>31 && c<127) {
				if (x>=video_num_columns) {
					x -= video_num_columns;
					pos -= video_size_row;
					lf();
				}
				__asm__("movb _attr,%%ah\n\t"
					"movw %%ax,%1\n\t"
					::"a" (c),"m" (*(short *)pos)
					:"ax");
				pos += 2;
				x++;
			} else if (c==27)
				state=1;
			else if (c==10 || c==11 || c==12)
				lf();
			else if (c==13)
				cr();
			else if (c==ERASE_CHAR(tty))
				del();
			else if (c==8) {
				if (x) {
					x--;
					pos -= 2;
				}
			} else if (c==9) {
				c=8-(x&7);
				x += c;
				pos += c<<1;
				if (x>video_num_columns) {
					x -= video_num_columns;
					pos -= video_size_row;
					lf();
				}
				c=9;
			} else if (c==7)
				sysbeep();
			break;
		case 1:
			state=0;
			if (c=='[')
				state=2;
			else if (c=='E')
				gotoxy(0,y+1);
			else if (c=='M')
				ri();
			else if (c=='D')
				lf();
			else if (c=='Z')
				respond(tty);
			else if (x=='7')
				save_cur();
			else if (x=='8')
				restore_cur();
			break;
		case 2:
			for(npar=0;npar<NPAR;npar++)
				par[npar]=0;
			npar=0;
			state=3;
			if (ques=(c=='?'))
				break;
		case 3:
			if (c==';' && npar<NPAR-1) {
				npar++;
				break;
			} else if (c>='0' && c<='9') {
				par[npar]=10*par[npar]+c-'0';
				break;
			} else state=4;
		case 4:
			state=0;
			switch(c) {
				case 'G': case '`':
					if (par[0]) par[0]--;
					gotoxy(par[0],y);
					break;
				case 'A':
					if (!par[0]) par[0]++;
					gotoxy(x,y-par[0]);
					break;
				case 'B': case 'e':
					if (!par[0]) par[0]++;
					gotoxy(x,y+par[0]);
					break;
				case 'C': case 'a':
					if (!par[0]) par[0]++;
					gotoxy(x+par[0],y);
					break;
				case 'D':
					if (!par[0]) par[0]++;
					gotoxy(x-par[0],y);
					break;
				case 'E':
					if (!par[0]) par[0]++;
					gotoxy(0,y+par[0]);
					break;
				case 'F':
					if (!par[0]) par[0]++;
					gotoxy(0,y-par[0]);
					break;
				case 'd':
					if (par[0]) par[0]--;
					gotoxy(x,par[0]);
					break;
				case 'H': case 'f':
					if (par[0]) par[0]--;
					if (par[1]) par[1]--;
					gotoxy(par[1],par[0]);
					break;
				case 'J':
					csi_J(par[0]);
					break;
				case 'K':
					csi_K(par[0]);
					break;
				case 'L':
					csi_L(par[0]);
					break;
				case 'M':
					csi_M(par[0]);
					break;
				case 'P':
					csi_P(par[0]);
					break;
				case '@':
					csi_at(par[0]);
					break;
				case 'm':
					csi_m();
					break;
				case 'r'://设置滚屏上下边界
					if (par[0]) par[0]--;
					if (!par[1]) par[1] = video_num_lines;
					if (par[0] < par[1] &&
					    par[1] <= video_num_lines) {
						top=par[0];
						bottom=par[1];
					}
					break;
				case 's':
					save_cur();
					break;
				case 'u':
					restore_cur();
					break;
			}
		}
	}
	set_cursor();
}

void con_init(void) {
	register unsigned char a;
	char *display_desc = "????";
	char *display_ptr;

	video_num_columns = ORIG_VIDEO_COLS;
	video_size_row = video_num_columns * 2;
	video_num_lines = ORIG_VIDEO_LINES;
	video_page = ORIG_VIDEO_PAGE;
	video_erase_char = 0x0720; //擦除字符0x20,0x07为属性

	if (ORIG_VIDEO_MODE == 7) /* Is this a monochrome display? 是否单色模式*/
	{
		video_mem_start = 0xb0000;
		video_port_reg = 0x3b4;
		video_port_val = 0x3b5;
		if ((ORIG_VIDEO_EGA_BX & 0xff) != 0x10) {
			video_type = VIDEO_TYPE_EGAM;
			video_mem_end = 0xb8000;
			display_desc = "EGAm";
		} else {
			video_type = VIDEO_TYPE_MDA;
			video_mem_end = 0xb2000;
			display_desc = "*MDA";
		}
	} else /* If not, it is color. 彩色模式*/
	{
		video_mem_start = 0xb8000;
		video_port_reg = 0x3d4;
		video_port_val = 0x3d5;
		if ((ORIG_VIDEO_EGA_BX & 0xff) != 0x10) {
			video_type = VIDEO_TYPE_EGAC;
			video_mem_end = 0xbc000;
			display_desc = "EGAc";
		} else {
			video_type = VIDEO_TYPE_CGA;
			video_mem_end = 0xba000;
			display_desc = "*CGA";
		}
	}
	//指针指向第一行倒数差4字节处
	display_ptr = ((char *) video_mem_start) + video_size_row - 8;
	//复制display_desc内容到display_ptr处
	while (*display_desc) {
		*display_ptr++ = *display_desc++;
		display_ptr++;
	}

	//初始化用于滚屏的变量
	origin = video_mem_start;
	scr_end = video_mem_start + video_num_lines * video_size_row;
	top = 0;
	bottom = video_num_lines;

	gotoxy(ORIG_X, ORIG_Y);
	set_trap_gate(0x21, &keyboard_interrupt);
	outb_p(inb_p(0x21)&0xfd, 0x21);
	//允许键盘中断
	a = inb_p(0x61);
	outb_p(a|0x80, 0x61);
	//禁用键盘
	outb(a, 0x61);
	//开启键盘（重置）
}



void sysbeepstop(void)
{
	/* disable counter 2 */
	outb(inb_p(0x61)&0xFC, 0x61);
}

int beepcount = 0;

/**
 * 打开蜂鸣，设置8255A&定时器通道2
 */
static void sysbeep(void)
{
	/* enable counter 2 */
	outb_p(inb_p(0x61)|3, 0x61);//0x61为8255A芯片PB端口，位1为扬声器开启信号，位0为定时器通道2信号
	/* set command for counter 2, 2 byte write */
	outb_p(0xB6, 0x43);
	/* send 0x637 for 750 HZ */
	outb_p(0x37, 0x42);
	outb(0x06, 0x42);
	/* 1/8 second */
	beepcount = HZ/8;
}
