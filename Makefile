####################################
# Makefile for playos              #
# author:ke.chenk                  #
# mail:lingqi1818@gmail.com        # 
####################################

AS86	=as86 -0 -a
LD86	=ld86 -0
AS		= as
LD		= ld
##LD_FLAGS	=-s -x
LDFLAGS	=-m elf_i386 -Ttext 0 -e startup_32
##LD86_FLAGS	= -0 -s -o
LD86_FLAGS	= -0 -o
CC	=gcc -march=i386 $(RAMDISK)
CFLAGS	=-c -g

ARCHIVES=fs/fs.o mm/mm.o lib/string.o kernel/kernel.o
DRIVERS =kernel/blk_drv/blk_drv.a kernel/chr_drv/chr_drv.a
LIBS	=lib/lib.a
PLAYOS_SECTS	= boot/bootsect boot/setup tools/system
OBJS 		= boot/head.o init/main.o
CPP	=cpp -nostdinc -Iinclude
# 避免当目标文件存在都时候，goal不执行
.PHONY : all clean

.c.s:
	$(CC) $(CFLAGS) \
	-nostdinc -Iinclude -S -o $*.s $<
.s.o:
	$(AS)  -o $*.o $<
.c.o:
	$(CC) $(CFLAGS) \
	-nostdinc -fno-builtin -Iinclude -O -o $*.o $<
	
default:
	@echo "please run with make image :-)"
# 默认执行所有
all: ${OBJS} ${PLAYOS_SECTS} 

clean :
	rm -rf ${OBJS} ${PLAYOS_SECTS} ${ARCHIVES} boot.img tools/build System.map ## 加上@符号就不会打印执行都命令本身
	(cd fs; make clean)
	(cd mm; make clean)
	(cd kernel; make clean)
	(cd lib;make clean)
## bootsect程序
boot/bootsect:	boot/bootsect.s
	$(AS86) -o boot/bootsect.o boot/bootsect.s
	$(LD86) -s -o boot/bootsect boot/bootsect.o
## setup程序
boot/setup: boot/setup.s
	$(AS86) -o boot/setup.o boot/setup.s
	$(LD86) -s -o boot/setup boot/setup.o

tools/system:	boot/head.o init/main.o $(ARCHIVES) $(DRIVERS) $(LIBS)
	$(LD) $(LDFLAGS)  boot/head.o init/main.o \
	$(ARCHIVES) \
	$(DRIVERS) \
	$(LIBS) \
	-o tools/system
	nm tools/system | grep -v '\(compiled\)\|\(\.o$$\)\|\( [aU] \)\|\(\.\.ng$$\)\|\(LASH[RL]DI\)'| sort > System.map 
	
kernel/chr_drv/chr_drv.a:
	(cd kernel/chr_drv; make)

kernel/blk_drv/blk_drv.a:
	(cd kernel/blk_drv; make)

fs/fs.o:
	(cd fs; make)
	
lib/lib.a:
	(cd lib; make)
	
mm/mm.o:
	(cd mm; make)
	
kernel/kernel.o:
	(cd kernel; make)
	
tools/build: tools/build.c
	gcc -mcpu=i386 $(RAMDISK) -Wall -O2 -fomit-frame-pointer  \
	-o tools/build tools/build.c	
	
Image: boot/bootsect boot/setup tools/system tools/build
	objcopy -O binary -R .note -R .comment tools/system tools/kernel
	tools/build boot/bootsect boot/setup tools/kernel $(ROOT_DEV) > Image
	rm tools/kernel -f
	sync
	
image: clean all buildimg

buildimg:Image
	dd bs=8192 if=Image of=boot.img
	sync
	

dep:
	sed '/\#\#\# Dependencies/q' < Makefile > tmp_make
	(for i in init/*.c;do echo -n "init/";$(CPP) -M $$i;done) >> tmp_make
	cp tmp_make Makefile
	(cd fs; make dep)
	(cd mm; make dep)
	(cd kernel; make dep)
	(cd lib;make dep)
	
### Dependencies:
init/main.o: init/main.c include/unistd.h include/sys/types.h \
 include/linux/kernel.h include/linux/fs.h include/asm/system.h \
 include/linux/sched.h include/linux/head.h include/linux/mm.h \
 include/signal.h
