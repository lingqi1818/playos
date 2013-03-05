#include <linux/fs.h>
#include <asm/system.h>
struct super_block super_block[NR_SUPER];
int ROOT_DEV = 0;

static void lock_super(struct super_block * sb)
{
	cli();
	while (sb->s_lock)
		sleep_on(&(sb->s_wait));
	sb->s_lock = 1;
	sti();
}

static void free_super(struct super_block * sb)
{
	cli();
	sb->s_lock = 0;
	wake_up(&(sb->s_wait));
	sti();
}

static void wait_on_super(struct super_block * sb)
{
	cli();
	while (sb->s_lock)
		sleep_on(&(sb->s_wait));
	sti();
}

void mount_root(void)
{
	int i,free;
		struct super_block * p;
		struct m_inode * mi;

		if (32 != sizeof (struct d_inode))
			panic("bad i-node size");
		for(i=0;i<NR_FILE;i++)
			file_table[i].f_count=0;
		if (MAJOR(ROOT_DEV) == 2) {
			printk("Insert root floppy and press ENTER");
			//wait_for_keypress();
		}
}


struct super_block * get_super(int dev)
{
	struct super_block * s;

	if (!dev)
		return NULL;
	s = 0+super_block;
	while (s < NR_SUPER+super_block)
		if (s->s_dev == dev) {
			wait_on_super(s);
			if (s->s_dev == dev)
				return s;
			s = 0+super_block;
		} else
			s++;
	return NULL;
}
