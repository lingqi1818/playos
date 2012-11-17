####################################
# Makefile for playos              #
# author:ke.chenk                  #
# mail:lingqi1818@gmail.com        # 
####################################

ASM		= as86
ASM_LD		= ld86 -0
AS		= as
LD		= ld
##LD_FLAGS	=-s -x
LD_FLAGS	=-x
ASMFLAGS	= -0 -a -o
##LD86_FLAGS	= -0 -s -o
LD86_FLAGS	= -0 -o
CC		= gcc
CFLAGS		= -c

ARCHIVES=fs/fs.o mm/mm.o lib/string.o kernel/kernel.o
PLAYOS_SECTS	= boot/bootsect.bin boot/setup.bin tools/system
OBJS 		= boot/bootsect.o boot/setup.o boot/head.o init/main.o
CPP	=cpp -nostdinc -Iinclude
# 避免当目标文件存在都时候，goal不执行
.PHONY : all clean

.c.o:
	$(CC) $(CFLAGS) \
	-nostdinc -fno-builtin -Iinclude -O -o $*.o $<

default:
	@echo "please run with make image :-)"
# 默认执行所有
all: ${OBJS} ${PLAYOS_SECTS} 

clean :
	rm -rf ${OBJS} ${PLAYOS_SECTS} ${ARCHIVES} boot.img System.map ## 加上@符号就不会打印执行都命令本身
	(cd fs; make clean)
	(cd mm; make clean)
	(cd kernel; make clean)
## bootsect程序
boot/bootsect.o : boot/bootsect.s
	${ASM} ${ASMFLAGS} $@ $<  ## $<代表依赖都目标，这里为:boot/bootsect.o,$@代表目标，这里为:bootsect.o

boot/bootsect.bin:
	${ASM_LD} ${LD86_FLAGS} $@ boot/bootsect.o

## setup程序
boot/setup.o : boot/setup.s
	${ASM} ${ASMFLAGS} $@ $<  ## $<代表依赖都目标，这里为:boot/setup.o,$@代表目标，这里为:setup.o

boot/setup.bin:
	${ASM_LD} ${LD86_FLAGS} $@ boot/setup.o

## head程序
boot/head.o : boot/head.s

tools/system:	boot/head.o init/main.o $(ARCHIVES)
	$(LD) $(LDFLAGS) -m elf_i386 -Ttext 0 -e startup_32  boot/head.o init/main.o \
	$(ARCHIVES) \
	-o tools/system

fs/fs.o:
	(cd fs; make)
	
mm/mm.o:
	(cd mm; make)
	
kernel/kernel.o:
	(cd kernel; make)
	
image: clean all buildimg

buildimg:
	dd bs=32 if=boot/bootsect.bin of=boot.img skip=1 ##dd命令，将指定文件写出磁盘 skip=1跳过头1个block，因为该32字节为ld86为minix专用
	dd bs=32 if=boot/setup.bin of=boot.img skip=1 seek=16 ##从文件都第513个字节开始写
	dd bs=512 if=tools/system of=boot.img skip=8 seek=5 ##前5个扇区为bootsect和setup
	sync
	

dep:
	sed '/\#\#\# Dependencies/q' < Makefile > tmp_make
	(for i in init/*.c;do echo -n "init/";$(CPP) -M $$i;done) >> tmp_make
	cp tmp_make Makefile
	(cd fs; make dep)
	(cd mm; make dep)
	(cd kernel; make dep)
	
### Dependencies:
init/main.o: init/main.c include/linux/fs.h
