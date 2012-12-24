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
OBJS=	tty_io.o

.c.s:
	$(CC) $(CFLAGS) \
	-nostdinc -I../../include -S -o $*.s $<
.s.o:
	$(AS) -o  $*.o $<

.c.o:
	$(CC) $(CFLAGS) \
	-nostdinc -I../../include -c -o $*.o $<
	
chr_drv.a: $(OBJS)
	$(AR) rcs chr_drv.a $(OBJS)
	sync
	
dep:
	sed '/\#\#\# Dependencies/q' < Makefile > tmp_make
	(for i in *.c;do $(CPP) -M $$i;done) >> tmp_make
	cp tmp_make Makefile
	
### Dependencies:
tty_io.o: tty_io.c
