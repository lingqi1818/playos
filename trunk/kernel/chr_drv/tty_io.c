#include <linux/tty.h>

/**
 * 目前 channel只实现了控制台类型，串口终端等未实现
 */

struct tty_struct tty_table[] = {
		{
			ICRNL,
			OPOST | ONLCR,
			0
		}

}; //tty表，目前只有一个成员：控制台类型




void tty_init(void)
{
	con_init();
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

	}

}

void chr_dev_init(void)
{
}
