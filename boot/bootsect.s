!
! 引导设备引导区代码，注意，本程序编译完之后生成的内容最多不能超过512K
! author:ke.chenk             		            
! mail:lingqi1818@gmail.com


.global begtext, begdata, begbss, endtext, enddata, endbss
.text
begtext:
.data
begdata:
.bss
begbss:
.text
BOOTSEG = 0X07c0 ! BIOS加载boot代码到指定的内存地址（原始段地址）（31k） 0x7c0是boot所在的段值，或者说单位是“节”（16字节）。换算成基地址值需要乘16，即0x7c00
! 总共有20根地址线也就是20位寻址.
! 0x07c0 是表示高16位地址位.
! 基地址是等于0x07c0加上四位二进制的位也就是一个十六进制的值=0x07c00
INITSEG	= 0x9000 ;将bootsec移动到这里
entry start !告知链接程序，程序从start处开始执行
start:
	mov	ax,#BOOTSEG
	mov	ds,ax
	mov	ax,#INITSEG
	mov	es,ax
	mov	cx,#256	! 每次移动2个字节，引导扇区一共512个字节，所以移动256次
	sub	si,si
	sub	di,di
	rep	! 重复执行后面一条指令，并递减cx,将bootsec从ds:si=0x7c00:0000移动到es:di=0x9000:0000处
	movw
	jmpi	go,#INITSEG	! 段间跳转
	
go:	mov	ax,cs
	mov	ds,ax
	mov	es,ax
	mov	ss,ax
	mov	sp,#0xFF00	! 将堆栈指针指向内存0x9000:0xff00处，即0x9ff00处
	
die:	jmp die
.org	510
	.word	0xAA55
.text
endtext:
.data
enddata:
.bss
endbss:
