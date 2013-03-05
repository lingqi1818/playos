####################################
# Makefile for playos's kernel     #
# author:ke.chenk                  #
# mail:lingqi1818@gmail.com        # 
####################################

LD	=ld
LD_FLAGS	=-x -r
CPP	=gcc -E -nostdinc -I../include
OBJS=	traps.o printk.o printf.o vsprintf.o sched.o system_call.o panic.o
CC		= gcc
## -fno-stack-protector去掉对栈的保护，否则可能出现：undefined reference to `__stack_chk_fail'
CFLAGS		= -c -fno-stack-protector -g

.c.s:
	$(CC) $(CFLAGS) \
	-nostdinc -I../include -S -o $*.s $<
.s.o:
	$(AS)  -o  $*.o $<
	
.c.o:
	$(CC) $(CFLAGS) \
	-nostdinc -fno-builtin -I../include -O -o $*.o $<
	
kernel.o: $(OBJS)
	$(LD) ${LD_FLAGS} -o kernel.o $(OBJS)
	
clean:
	rm -rf $(OBJS)
	(cd chr_drv; make clean)
	(cd blk_drv; make clean)
	
dep:
	sed '/\#\#\# Dependencies/q' < Makefile > tmp_make
	(for i in *.c;do echo -n `echo $$i | sed 's,\.c,\.s,'`" "; \
		$(CPP) -M $$i;done) >> tmp_make
	cp tmp_make Makefile
	(cd chr_drv; make dep)
	(cd blk_drv; make dep)

### Dependencies:
panic.s panic.o: panic.c ../include/linux/kernel.h ../include/linux/sched.h \
 ../include/linux/fs.h ../include/sys/types.h ../include/linux/head.h \
 ../include/linux/mm.h ../include/signal.h
printf.s printf.o: printf.c ../include/stdarg.h
printk.s printk.o: printk.c ../include/stdarg.h
sched.s sched.o: sched.c ../include/linux/sched.h ../include/linux/fs.h \
 ../include/sys/types.h ../include/linux/head.h ../include/linux/mm.h \
 ../include/signal.h ../include/linux/kernel.h ../include/asm/system.h \
 ../include/asm/io.h ../include/linux/sys.h
traps.s traps.o: traps.c ../include/asm/system.h
vsprintf.s vsprintf.o: vsprintf.c ../include/stdarg.h ../include/string.h
