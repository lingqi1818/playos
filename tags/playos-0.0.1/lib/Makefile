############################################
# Makefile for playos's lib       	       #
# author:ke.chenk                          #
# mail:lingqi1818@gmail.com                #
############################################
AR	=ar
AS	=as
LD	=ld
LD_FLAGS	=-x -r
CPP	=gcc -E -nostdinc -I../include
CC		= gcc
CFLAGS		= -c -g
OBJS=	ctype.o errno.o write.o

.c.s:
	$(CC) $(CFLAGS) \
	-nostdinc -fno-builtin -I../include -S -O -o $*.s $<
.s.o:
	$(AS) -o  $*.o $<

.c.o:
	$(CC) $(CFLAGS) \
	-nostdinc -fno-builtin -I../include -O -o $*.o $<

lib.a: $(OBJS)
	$(AR) rcs lib.a $(OBJS)
	sync

clean:
	rm -f core *.o *.a tmp_make
	for i in *.c;do rm -f `basename $$i .c`.s;done

dep:
	sed '/\#\#\# Dependencies/q' < Makefile > tmp_make
	(for i in *.c;do echo -n `echo $$i | sed 's,\.c,\.s,'`" "; \
		$(CPP) -M $$i;done) >> tmp_make
	cp tmp_make Makefile

### Dependencies:
ctype.s ctype.o: ctype.c ../include/ctype.h
errno.s errno.o: errno.c
write.s write.o: write.c ../include/unistd.h ../include/sys/types.h
