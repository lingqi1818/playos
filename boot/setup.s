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

	call	empty_8042
	mov	al,#0xd1	! 写数据到8042的p2端口,p2的位1用于A20线的选通
	out	#0x64,al
	call	empty_8042	! 等待缓冲区为空，命令被接受
	mov	al,#0xdf	! 选通A20线
	out	#0x60,al
	call	empty_8042

	! icw1
	mov	al,#0x11	! 需要ICW4，级联，边沿触发，8字节中断向量
	out	#0x20,al
	.word	0x00eb,0x00eb	! 每个0x00eb耗费CPU7-10个时钟周期延迟，如果用nop指令则耗费3个
	out	#0xa0,al
	.word	0x00eb,0x00eb
	
	! icw2
	mov	al,#0x20	! 主芯片，中断号从0x20开始
	out	#0x21,al
	.word	0x00eb,0x00eb
	mov	al,#0x28	! 从芯片，中断号从0x28开始
	out	#0xa1,al
	.word	0x00eb,0x00eb

	! icw3
	mov	al,#0x04	! IR2级联从片
	out	#0x21,al
	.word	0x00eb,0x00eb
	mov	al,#0x02	! 主片都IR号
	out	#0xa1,al
	.word	0x00eb,0x00eb
	
	! icw4
	mov	al,#0x01	! 80x86模式,普通EOI，非缓冲方式
	out	#0x21,al
	.word	0x00eb,0x00eb
	out	#0xa1,al
	.word	0x00eb,0x00eb

	mov	al,#0xff
	out	#0x21,al	! 屏蔽主芯片所有中断
	.word	0x00eb,0x00eb	! 屏蔽从芯片所有中断
	out	#0xa1,al
	
	mov	ax,#0x0001
	lmsw	ax
	jmpi	0,8

! 只有当8042键盘控制器的输入缓冲器为空时（状态寄存器位1=0）,才可以对其进行写命令
empty_8042:
	.word	0x00eb,0x00eb	! 跳转指令机器码（跳转到下一句），相当于延时
	in	al,#0x64	! 8042 status port
	test	al,#2
	jnz	empty_8042	! 不为空，进入死循环
	ret
	
gdt:
	.word	0,0,0,0
	.word	0x07ff	! 段界限为2048*4K=8M
	.word	0x0000	! 基地址为0
	.word	0x9a00	! 代码段为只读，可执行
	.word	0x00c0	! 颗粒读为4K，32位模式

	.word	0x07ff	! 段界限为2048*4K=8M
	.word	0x0000	! 基地址为0
	.word	0x9200	! 数据段，可读写
	.word	0x00c0	! 颗粒读为4K，32位模式
	
! cpu进入保护模式需要设置IDT，先设置个空表
idt_48:
	.word	0	! idt limit
	.word	0,0	! idt base，

gdt_48:
	.word	0x800	! gdt limit 2K，256 gdt entries
	.word	512+gdt,0x9	! gdt base 0x90200+gdt
.text
endtext:
.data
enddata:
.bss
endbss:
