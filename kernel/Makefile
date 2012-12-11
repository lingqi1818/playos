####################################
# Makefile for playos's kernel     #
# author:ke.chenk                  #
# mail:lingqi1818@gmail.com        # 
####################################

LD	=ld
LD_FLAGS	=-x -r
CPP	=gcc -E -nostdinc -I../include
OBJS=	traps.o printk.o vsprintf.o sched.o system_call.o
CC		= gcc
## -fno-stack-protector去掉对栈的保护，否则可能出现：undefined reference to `__stack_chk_fail'
CFLAGS		= -c -fno-stack-protector 
.c.o:
	$(CC) $(CFLAGS) \
	-nostdinc -fno-builtin -I../include -O -o $*.o $<
	
kernel.o: $(OBJS)
	$(LD) ${LD_FLAGS} -o kernel.o $(OBJS)
	
clean:
	rm -rf $(OBJS)
	
dep:
	sed '/\#\#\# Dependencies/q' < Makefile > tmp_make
	(for i in *.c;do echo -n `echo $$i | sed 's,\.c,\.s,'`" "; \
		$(CPP) -M $$i;done) >> tmp_make
	cp tmp_make Makefile

### Dependencies:
printk.s printk.o: printk.c ../include/stdarg.h
sched.s sched.o: sched.c ../include/linux/sched.h ../include/linux/head.h \
 ../include/linux/mm.h ../include/signal.h ../include/asm/system.h \
 ../include/sys/types.h ../include/asm/io.h ../include/linux/sys.h
traps.s traps.o: traps.c ../include/asm/system.h
vsprintf.s vsprintf.o: vsprintf.c ../include/stdarg.h ../include/string.h
