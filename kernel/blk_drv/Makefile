############################################
# Makefile for playos's kernel/blk_drv     #
# author:ke.chenk                          #
# mail:lingqi1818@gmail.com                #
############################################
AR	=ar
AS	=as
LD	=ld
LDFLAGS	= -x
CC	=gcc
CFLAGS		= -c -g
CPP	=gcc -E -nostdinc -I../../include

.c.s:
	$(CC) $(CFLAGS) \
	-nostdinc -I../../include -S -o $*.s $<
.s.o:
	$(AS) -o  $*.o $<

.c.o:
	$(CC) $(CFLAGS) \
	-nostdinc -I../../include -c -o $*.o $<

OBJS  = ll_rw_blk.o hd.o

blk_drv.a: $(OBJS)
	$(AR) rcs blk_drv.a $(OBJS)
	sync

clean:
	rm -f $(OBJS)
	rm -f blk_drv.a

dep:
	sed '/\#\#\# Dependencies/q' < Makefile > tmp_make
	(for i in *.c;do echo -n `echo $$i | sed 's,\.c,\.s,'`" "; \
		$(CPP) -M $$i;done) >> tmp_make
	cp tmp_make Makefile

### Dependencies:
hd.s hd.o: hd.c ../../include/linux/sched.h ../../include/linux/head.h \
 ../../include/linux/mm.h ../../include/signal.h ../../include/asm/io.h \
 ../../include/linux/hdreg.h ../../include/asm/system.h \
 ../../include/linux/fs.h ../../include/sys/types.h blk.h
ll_rw_blk.s ll_rw_blk.o: ll_rw_blk.c blk.h ../../include/sys/types.h \
 ../../include/linux/fs.h ../../include/asm/system.h
