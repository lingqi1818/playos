!
! 利用BIOS中断，读取机器系统数据，存入0x90000开始都内存位置处，也就是覆盖bootsect
! 然后将system模块从0x10000位置挪到0x00000处
! author:ke.chenk             		            
! mail:lingqi1818@gmail.com
!

INITSEG	= 0X9000
SYSSEG	= 0X1000
SETUPSEG = 0X9020
.global begtext, begdata, begbss, endtext, enddata, endbss
.text
begtext:
.data
begdata:
.bss
begbss:
.text
entry start
start:
	mov	ax,#INITSEG
	mov	ds,ax
	! 功能03H
	! 功能描述：在文本坐标下，读取光标各种信息
	! 入口参数：AH＝03H
	! BH＝显示页码
	! 出口参数：CH＝光标的起始行
	! CL＝光标的终止行
	! DH＝行(Y坐标)
	! DL＝列(X坐标)
	mov	ah,#0x03	
	xor	bh,bh
	int	0x10
	mov	[0],dx	! 保存光标位置

	! 杂项系统服务(Miscellaneous System Service——INT 15H)
	! 功能描述：读取扩展内存大小
	! 入口参数：AH＝88H
	! 出口参数：AX＝扩展内存字节数(以K为单位)
	mov	ah,#0x88
	int	0x15
	mov	[2],ax
	
	! int 0x10 功能0FH
	! 功能描述：读取显示器模式
	! 入口参数：AH＝0FH
	! 出口参数：AH＝屏幕字符的列数
	! AL＝显示模式(参见功能00H中的说明)
	! BH＝页码
	mov	ah,#0x0f
	int	0x10
	mov	[4],bx
	mov	[6],ax

	! 检查EGA/VGA显示方式，目前都显卡适配器可以支持吗？？？
	mov	ah,#0x12
	mov	bl,#10 ! 功能号10H — 读取配置信息
	int	0x10
	mov	[8],ax
	mov	[10],bx	! bl显存，bh显示状态
	mov	[12],cx	! 显卡特性参数

	! 获取硬盘信息，第一个硬盘参数表首地址为中断向量0x41的向量值（中断向量表中的每一项为向量号*4，每项占用4个字节）
	! 硬盘信息表都长度为16个字节
	mov	ax,0x0000
	mov	ds,ax
	lds	si,[4*0x41]
	mov	ax,#INITSEG
	mov	es,ax
	mov	di,#0x0080	! 传送地址,0x90080
	mov	cx,#0x10
	rep
	movsb
	
	! 获取第二个磁盘信息
	mov	ax,0x0000
	mov	ds,ax
	lds	si,[4*0x46]
	mov	ax,#INITSEG
	mov	es,ax
	mov	di,#0x0090	! 传送地址,0x90090
	mov	cx,#0x10
	rep
	movsb

	! 检查系统是否有第二个硬盘，如果没有则把第二个表清0
	! 功能描述：读取磁盘类型
	! 入口参数：AH＝15H
	! DL＝驱动器，00H~7FH：软盘；80H~0FFH：硬盘
	! 出口参数：CF＝1——操作失败，AH＝状态代码，参见功能号01H中的说明， 否则，AH＝00H — 未安装驱动器
	! ＝01H — 无改变线支持的软盘驱动器
	! ＝02H — 带有改变线支持的软盘驱动器
	! ＝03H — 硬盘，CX:DX＝512字节的扇区数
	mov	ax,#0x01500
	mov	dl,#0x81	! 第二个硬盘号
	int	0x13
	jc	no_disk1
	cmp	ah,#3	! 是否硬盘类型
	je	is_disk1
no_disk1:
	mov	ax,#INITSEG
	mov	es,ax
	mov	di,#0x0090
	mov	cx,#0x10
	mov	ax,#0x00
	rep
	stosb	! 将ax的值存入到es:di指向的地址
is_disk1:

! 现在开始准备进入保护模式，从此开始禁止中断
	cli
	mov	ax,#0x0000
	cld	! 清方向标志位，让si,di指针归零
do_move:
	mov	es,ax
	add	ax,#0x1000
	cmp	ax,#0x9000
	jz	end_move
	mov	ds,ax
	sub	di,di
	sub	si,si
	mov	cx,#0x8000 !每次移动32K字，即64K字节
	rep
	movsw
	jmp	do_move
end_move:
	mov	ax,#SETUPSEG
	mov	ds,ax
	lidt	idt_48
	lgdt	gdt_48
	
! cpu进入保护模式需要设置IDT，先设置个空表
idt_48:
	.word	0	! idt limit
	.word	0,0	! idt base

gdt_48:
die:jmp die
.text
endtext:
.data
enddata:
.bss
endbss:
