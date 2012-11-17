####################################
# Makefile for playos's mm         #
# author:ke.chenk                  #
# mail:lingqi1818@gmail.com        # 
####################################

LD	=ld
LD_FLAGS	=-x -r
CPP	=gcc -E -nostdinc -I../include
OBJS=	memory.o
CC		= gcc
CFLAGS		= -c

.c.o:
	$(CC) $(CFLAGS) \
	-nostdinc -fno-builtin -I../include -O -o $*.o $<
	
mm.o: $(OBJS)
	$(LD) ${LD_FLAGS} -o mm.o $(OBJS)

clean:
	rm -rf $(OBJS)
	
dep:
	sed '/\#\#\# Dependencies/q' < Makefile > tmp_make
	(for i in *.c;do $(CPP) -M $$i;done) >> tmp_make
	cp tmp_make Makefile
	
### Dependencies:
memory.o: memory.c
