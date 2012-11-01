####################################
# Makefile for playos's fs         #
# author:ke.chenk                  #
# mail:lingqi1818@gmail.com        # 
####################################

LD	=ld
LD_FLAGS	=-x -M
CPP	=gcc -E -nostdinc -I../include
OBJS=	super.o
	
fs.o: $(OBJS)
	$(LD) ${LD_FLAGS} -o fs.o $(OBJS)
	
dep:
	sed '/\#\#\# Dependencies/q' < Makefile > tmp_make
	(for i in *.c;do $(CPP) -M $$i;done) >> tmp_make
	cp tmp_make Makefile
	
### Dependencies:
super.o: super.c
