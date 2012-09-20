####################################
# Makefile for playos              #
# author:ke.chenk                  #
# mail:lingqi1818@gmail.com        # 
####################################

ASM		= as86
ASM_LD		= ld86
ASMFLAGS	= -0 -a -o
LDFLAGS		= -0 -s -o

PLAYOS_BOOTS	= boot/boot.bin
OBJS 		= boot/boot.o

# 避免当目标文件存在都时候，goal不执行
.PHONY : all clean

# 默认执行所有
all: ${OBJS} ${PLAYOS_BOOTS} 

clean :
	@rm -rf ${OBJS} ${PLAYOS_BOOTS} ## 加上@符号就不会打印执行都命令本身

## boot程序
boot/boot.o : boot/boot.s
	@${ASM} ${ASMFLAGS} $@ $<  ## $<代表依赖都目标，这里为:boot/boot.o,$@代表目标，这里为:boot.bin

boot/boot.bin:
	@${ASM_LD} ${LDFLAGS} $@ boot/boot.o
	


image: clean all buildimg

buildimg:
	@dd bs=32 if=boot/boot.bin of=boot.img skip=1 ##dd命令，将指定文件写出磁盘
