####################################
# Makefile for playos's fs         #
# author:ke.chenk                  #
# mail:lingqi1818@gmail.com        # 
####################################

LD	=ld
LD_FLAGS	=-x -r
CPP	=gcc -E -nostdinc -I../include
OBJS=	super.o read_write.o buffer.o file_table.o inode.o namei.o truncate.o bitmap.o
CC		= gcc
CFLAGS		= -c -g

.c.s:
	$(CC) $(CFLAGS) \
	-nostdinc -I../include -S -o $*.s $<
.s.o:
	$(AS)  -o  $*.o $<
	
.c.o:
	$(CC) $(CFLAGS) \
	-nostdinc -fno-builtin -I../include -O -o $*.o $<
	
fs.o: $(OBJS)
	$(LD) ${LD_FLAGS} -o fs.o $(OBJS)

clean:
	rm	-rf $(OBJS)
dep:
	sed '/\#\#\# Dependencies/q' < Makefile > tmp_make
	(for i in *.c;do $(CPP) -M $$i;done) >> tmp_make
	cp tmp_make Makefile
	
### Dependencies:
bitmap.o: bitmap.c ../include/string.h ../include/linux/sched.h \
 ../include/linux/fs.h ../include/sys/types.h ../include/linux/head.h \
 ../include/linux/mm.h ../include/signal.h ../include/linux/kernel.h
buffer.o: buffer.c ../include/linux/fs.h ../include/sys/types.h \
 ../include/asm/system.h ../include/asm/io.h
file_table.o: file_table.c ../include/linux/fs.h ../include/sys/types.h
inode.o: inode.c ../include/linux/sched.h ../include/linux/fs.h \
 ../include/sys/types.h ../include/linux/head.h ../include/linux/mm.h \
 ../include/signal.h
namei.o: namei.c ../include/linux/fs.h ../include/sys/types.h \
 ../include/linux/sched.h ../include/linux/head.h ../include/linux/mm.h \
 ../include/signal.h
open.o: open.c
read_write.o: read_write.c ../include/linux/kernel.h
super.o: super.c ../include/linux/fs.h ../include/sys/types.h
truncate.o: truncate.c ../include/linux/sched.h ../include/linux/fs.h \
 ../include/sys/types.h ../include/linux/head.h ../include/linux/mm.h \
 ../include/signal.h
