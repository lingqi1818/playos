############################################
# Makefile for playos's kernel/chr_drv     #
# author:ke.chenk                          #
# mail:lingqi1818@gmail.com                #
############################################
AR	=ar
AS	=as
LD	=ld
LD_FLAGS	=-x -r
CPP	=gcc -E -nostdinc -I../../include
CC		= gcc
CFLAGS		= -c -g
OBJS=	tty_io.o console.o keyboard.o

.c.s:
	$(CC) $(CFLAGS) \
	-nostdinc -fno-builtin -I../../include -S -O -o $*.s $<
.s.o:
	$(AS) -o  $*.o $<

.c.o:
	$(CC) $(CFLAGS) \
	-nostdinc -fno-builtin -I../../include -O -o $*.o $<
	
chr_drv.a: $(OBJS)
	$(AR) rcs chr_drv.a $(OBJS)
	sync

##预编译
keyboard.s: keyboard.S ../../include/linux/config.h
	$(CPP) -traditional keyboard.S -o keyboard.s


clean:
	rm -f $(OBJS)
	rm -f chr_drv.a
	
dep:
	sed '/\#\#\# Dependencies/q' < Makefile > tmp_make
	(for i in *.c;do $(CPP) -M $$i;done) >> tmp_make
	cp tmp_make Makefile
	
### Dependencies:
console.o: console.c ../../include/linux/sched.h \
 ../../include/linux/head.h ../../include/linux/mm.h \
 ../../include/signal.h ../../include/linux/tty.h ../../include/termios.h \
 ../../include/asm/io.h ../../include/asm/system.h
tty_io.o: tty_io.c ../../include/ctype.h ../../include/linux/tty.h \
 ../../include/termios.h ../../include/signal.h \
 ../../include/linux/sched.h ../../include/linux/head.h \
 ../../include/linux/mm.h
