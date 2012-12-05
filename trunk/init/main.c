#define __LIBRARY__
#include <unistd.h>

static inline _syscall0(int,fork)

#include <linux/fs.h>
#include <asm/system.h>
#include <linux/sched.h>
#define EXT_MEM_K (*(unsigned short *)0x90002)	//扩展内存大小
#define ORIG_ROOT_DEV (*(unsigned short *)0x901FC)//ROOT_DEV设备号，在bootsect中定义（508字节处），由于bootsect被加载到0x90000~~0x901ff处，所以ROOT_DEV地址为0x901FC。
#define DRIVE_INFO (*(struct drive_info *)0x90080)	//第一个硬盘信息
struct drive_info {
	char dummy[32];
} drive_info;//硬盘参数表

void write_char(char);
void start(){
		write_char('P');
		write_char('l');
		write_char('a');
		write_char('y');
		write_char(' ');
		write_char('O');
		write_char('S');
		write_char(' ');
}
static long memory_end=0;
static long	buffer_memory_end=0;//高速缓冲区末端
static long	main_memory_start=0;//主内存开始位置（用于分页）
void main(void)
{
	start();
	ROOT_DEV = ORIG_ROOT_DEV;
	drive_info = DRIVE_INFO;
	memory_end = (1<<20)+(EXT_MEM_K<<10); //内存大小=1M+扩展内存（K）*1024 字节
	memory_end &= 0xfffff000;//忽略不到4K的内存（1页）
	if (memory_end > 16*1024*1024)//内存超过16M，则按16M算
		memory_end = 16*1024*1024;
	if (memory_end > 12*1024*1024)	//缓冲区设置为内存都1/4
		buffer_memory_end = 4*1024*1024;
	else if (memory_end > 6*1024*1024)
		buffer_memory_end = 2*1024*1024;
	else
		buffer_memory_end = 1*1024*1024;
		main_memory_start =	buffer_memory_end;//主内存起始位置=缓冲区末端
		//TODO 虚拟盘都问题以后再做研究

		mem_init(main_memory_start,memory_end);//主内存初始化
		trap_init();//硬件中断向量初始化
		sched_init();
		//TODO,其他模块初始化
		sti();
		move_to_user_mode();//从内核态进入用户态，init进程开始

}
