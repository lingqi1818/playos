/**
 * 说明：内存0-1M处为内核和显存以及BIOS(640K-1M)
 * 1M以上，在start_mem之前为高速缓冲区，之后为物理内存
 * 初始化时，start_mem之前都内存分页都为USED，之后都设置为0
 *
 * author:ke.chenk
 * mail:lingqi1818@gmail.com
 *
 */
#define LOW_MEM 0x100000 //内存低端1M处
#define	PAGING_MEMORY (15*1024*1024) //分页内存15M，主内存区最多15M
#define PAGING_PAGES (PAGING_MEMORY>>12)//分页后物理内存页面数（3840）
#define MAP_NR(addr) (((addr)-LOW_MEM)>>12)//指定内存地址映射为页号
#define USED 100
static long HIGH_MEMORY = 0;
static unsigned char mem_map [ PAGING_PAGES ] = {0,};
void mem_init(long start_mem, long end_mem){

	int i;
	HIGH_MEMORY=end_mem;
	for (i=0 ; i<PAGING_PAGES ; i++)
	mem_map[i] = USED;
	i=MAP_NR(start_mem);//主内存起始页号
	end_mem-=start_mem;
	end_mem>>=12;
	while(end_mem-->0){
		mem_map[i++]=0;
	}

}


void free_page(unsigned long addr)
{
}
