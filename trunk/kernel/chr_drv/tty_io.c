#include <linux/tty.h>
#include <signal.h>
#define ALRMMASK (1<<(SIGALRM-1))
#define KILLMASK (1<<(SIGKILL-1))
#define INTMASK (1<<(SIGINT-1))
#define QUITMASK (1<<(SIGQUIT-1))
#define TSTPMASK (1<<(SIGTSTP-1))

/**
 * 目前 channel只实现了控制台类型，串口终端等未实现
 */

struct tty_struct tty_table[] = {
		{ICRNL,	/* input mode flags 将CR转换成NL */
		OPOST|ONLCR,/* output mode flags 输出模式，将NL转成CR*/
		ISIG | ICANON | ECHO | ECHOCTL | ECHOKE/* local mode flags */},//termios
		0,//所属进程组
		0,//停止标记
		con_write,//写函数指针
		{0,0,0,0,""},//读队列
		{0,0,0,0,""},//写队列
		{0,0,0,0,""}//辅助队列，存放键盘输入 规范熟模式内容

}; //tty表，目前只有一个成员：控制台类型



/**
 * tty初始化
 */
void tty_init(void)
{
	//初始化控制台，键盘功能在此唤醒
	con_init();
}

/**
 * 将键盘输入的tty->read_q内容转换成熟模式拷贝到tty->secondary队列中
 * 如果tty开启了本地回显模式，则直接tty->write
 */
void copy_to_cooked(struct tty_struct * tty)
{
	signed char c;

	while (!EMPTY(tty->read_q) && !FULL(tty->secondary)) {
		GETCH(tty->read_q,c);
		if (c==13)
			if (I_CRNL(tty))
				c=10;
			else if (I_NOCR(tty))
				continue;
			else ;
		else if (c==10 && I_NLCR(tty))
			c=13;
		if (I_UCLC(tty))
			c=tolower(c);
		if (L_CANON(tty)) {
			if (c==KILL_CHAR(tty)) {
				/* deal with killing the input line */
				while(!(EMPTY(tty->secondary) ||
				        (c=LAST(tty->secondary))==10 ||
				        c==EOF_CHAR(tty))) {
					if (L_ECHO(tty)) {
						if (c<32)
							PUTCH(127,tty->write_q);
						PUTCH(127,tty->write_q);
						tty->write(tty);
					}
					DEC(tty->secondary.head);
				}
				continue;
			}
			if (c==ERASE_CHAR(tty)) {
				if (EMPTY(tty->secondary) ||
				   (c=LAST(tty->secondary))==10 ||
				   c==EOF_CHAR(tty))
					continue;
				if (L_ECHO(tty)) {
					if (c<32)
						PUTCH(127,tty->write_q);
					PUTCH(127,tty->write_q);
					tty->write(tty);
				}
				DEC(tty->secondary.head);
				continue;
			}
			if (c==STOP_CHAR(tty)) {
				tty->stopped=1;
				continue;
			}
			if (c==START_CHAR(tty)) {
				tty->stopped=0;
				continue;
			}
		}
		if (L_ISIG(tty)) {
			if (c==INTR_CHAR(tty)) {
				tty_intr(tty,INTMASK);
				continue;
			}
			if (c==QUIT_CHAR(tty)) {
				tty_intr(tty,QUITMASK);
				continue;
			}
		}
		if (c==10 || c==EOF_CHAR(tty))
			tty->secondary.data++;
		if (L_ECHO(tty)) {
			if (c==10) {
				PUTCH(10,tty->write_q);
				PUTCH(13,tty->write_q);
			} else if (c<32) {
				if (L_ECHOCTL(tty)) {
					PUTCH('^',tty->write_q);
					PUTCH(c+64,tty->write_q);
				}
			} else
				PUTCH(c,tty->write_q);
			tty->write(tty);
		}
		PUTCH(c,tty->secondary);
	}
	wake_up(&tty->secondary.proc_list);
}


/**
 * channel:设备号，目前只支持控制台类型
 * buf:缓冲区
 * nr:写字节数
 */
int tty_write(unsigned channel, char *buf, int nr) {
	static cr_flag = 0;
	struct tty_struct * tty;
	char c, *b = buf;
	if (channel != 0)
		return -1; //0为控制台类型
	tty = tty_table + channel;
	while(nr){
		sleep_if_full(&tty->write_q);
				if (current->signal)
					break;
				while (nr>0 && !FULL(tty->write_q)) {
					c=get_fs_byte(b);
					if (O_POST(tty)) {
						if (c=='\r' && O_CRNL(tty))
							c='\n';
						else if (c=='\n' && O_NLRET(tty))
							c='\r';
						if (c=='\n' && !cr_flag && O_NLCR(tty)) {
							cr_flag = 1;
							PUTCH(13,tty->write_q);
							continue;
						}
						if (O_LCUC(tty))
							c=toupper(c);
					}
					b++; nr--;
					cr_flag = 0;
					PUTCH(c,tty->write_q);
				}
				tty->write(tty);
				if (nr>0)
					schedule();
			}
			return (b-buf);
}

/**
 * 选择需要处理的tty进行操作
 */
void do_tty_interrupt(int tty)
{
	copy_to_cooked(tty_table+tty);
}

void chr_dev_init(void)
{
}
