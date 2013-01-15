#include <linux/kernel.h>
int sys_write(unsigned int fd,char * buf,int count)
{
	return printk(buf);
}
