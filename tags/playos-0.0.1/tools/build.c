/*
 *本程序主要将以下三个文件生成磁盘映像文件
 * bootsec：启动扇区（512字节）
 * setup:设置系统参数，占用4个扇区
 * system:实际内核代码
 * 
 * autor:ke.chenk
 * mail:lingqi1818@msn.com
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <include/linux/fs.h>
#include <unistd.h>
#include <fcntl.h>


#define DEFAULT_MAJOR_ROOT 3
#define DEFAULT_MINOR_ROOT 6	// dev/hd6 第二个硬盘第一个分区


#define MINIX_HEADER 32
#define GCC_HEADER 1024
#define SYS_SIZE 0x2000
#define SETUP_SECTS 4

void die(char *str)
{
    fprintf(stderr,"%s\n",str);
    exit(1);
}

void usage(void)
{
    die("Usage: build bootsec setup system [rootdev] [> image]");
}
int main(int argc,char **argv)
{
    int i,c,id;
    char buf[1024];
    char major_root,minor_root;
    struct stat sb;
    if (argc!=4 && argc!=5)
        usage();
    
    if (argc==5)
    {
        if(strcmp(argv[4],"FLOPPY"))   
        {
            if(stat(argv[4],&sb))
            {
                perror(argv[4]);
                die("couldn't stat root device .");
            }
            major_root = MAJOR(sb.st_rdev);
            minor_root = MINOR(sb.st_rdev);
        }else{
	    major_root = 0;
            minor_root = 0;
	}
    }else{
	    major_root = DEFAULT_MAJOR_ROOT;
	    minor_root = DEFAULT_MINOR_ROOT;
	}
	fprintf(stderr, "Root device is (%d, %d)\n", major_root, minor_root);
	if ((major_root != 2) && (major_root != 3) &&
	    (major_root != 0)) {
		fprintf(stderr, "Illegal root device (major = %d)\n",
			major_root);
		die("Bad root device --- major #");
	}
	for (i=0;i<sizeof buf; i++) buf[i]=0;
	//read bootsect
	if ((id=open(argv[1],O_RDONLY,0))<0)
		die("Unable to open 'boot'");
	if (read(id,buf,MINIX_HEADER) != MINIX_HEADER)
		die("Unable to read header of 'boot'");
	if ((id=open(argv[1],O_RDONLY,0))<0)
		die("Unable to open 'boot'");
	if (read(id,buf,MINIX_HEADER) != MINIX_HEADER)
		die("Unable to read header of 'boot'");
	if (((long *) buf)[0]!=0x04100301)
		die("Non-Minix header of 'boot'");
	if (((long *) buf)[1]!=MINIX_HEADER)
		die("Non-Minix header of 'boot'");
	if (((long *) buf)[3]!=0)
		die("Illegal data segment in 'boot'");
	if (((long *) buf)[4]!=0)
		die("Illegal bss in 'boot'");
	if (((long *) buf)[5] != 0)
		die("Non-Minix header of 'boot'");
	if (((long *) buf)[7] != 0)
		die("Illegal symbol table in 'boot'");
	i=read(id,buf,sizeof buf);
	fprintf(stderr,"Boot sector %d bytes.\n",i);
	if (i != 512)
		die("Boot block must be exactly 512 bytes");
	if ((*(unsigned short *)(buf+510)) != 0xAA55)
	die("Boot block hasn't got boot flag (0xAA55)");
	buf[508] = (char) minor_root;
	buf[509] = (char) major_root;
	i=write(1,buf,512);
	if (i!=512)
		die("Write call failed");
	close (id);

	// read setup
	if ((id=open(argv[2],O_RDONLY,0))<0)
		die("Unable to open 'setup'");
	if (read(id,buf,MINIX_HEADER) != MINIX_HEADER)
		die("Unable to read header of 'setup'");
	if (((long *) buf)[0]!=0x04100301)
		die("Non-Minix header of 'setup'");
	if (((long *) buf)[1]!=MINIX_HEADER)
		die("Non-Minix header of 'setup'");
	if (((long *) buf)[3]!=0)
		die("Illegal data segment in 'setup'");
	if (((long *) buf)[4]!=0)
		die("Illegal bss in 'setup'");
	if (((long *) buf)[5] != 0)
		die("Non-Minix header of 'setup'");
	if (((long *) buf)[7] != 0)
		die("Illegal symbol table in 'setup'");
        for (i=0 ; (c=read(id,buf,sizeof buf))>0 ; i+=c )
		if (write(1,buf,c)!=c)
			die("Write call failed");
	close (id);
	if (i > SETUP_SECTS*512)
		die("Setup exceeds " STRINGIFY(SETUP_SECTS)
			" sectors - rewrite build/boot/setup");
	fprintf(stderr,"Setup is %d bytes.\n",i);
        //清空缓冲区
        for (c=0 ; c<sizeof(buf) ; c++)
		buf[c] = '\0';
	//若setup未满4个扇区，则余下部分补0
        while (i<SETUP_SECTS*512) {
		c = SETUP_SECTS*512-i;
		if (c > sizeof(buf))
			c = sizeof(buf);
		if (write(1,buf,c) != c)
			die("Write call failed");
		i += c;
	}
        
        //read system
        if ((id=open(argv[3],O_RDONLY,0))<0)
		die("Unable to open 'system'");
	if (read(id,buf,GCC_HEADER) != GCC_HEADER)
		die("Unable to read header of 'system'");
	if (((long *) buf)[5] != 0)
		die("Non-GCC header of 'system'");
	for (i=0 ; (c=read(id,buf,sizeof buf))>0 ; i+=c )
		if (write(1,buf,c)!=c)
			die("Write call failed");
	close(id);
	fprintf(stderr,"System is %d bytes.\n",i);
	if (i > SYS_SIZE*16)    //系统长度超过128KB则退出
		die("System is too big");
	return(0);
}
