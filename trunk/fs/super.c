#include <linux/fs.h>
int ROOT_DEV = 0;
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
