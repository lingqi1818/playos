!
! 引导设备引导区代码，注意，本程序编译完之后生成的内容最多不能超过512K
! author:ke.chenk             		            
! mail:lingqi1818@gmail.com
!
! 以下为BIOS中断—直接磁盘服务(Direct Disk Service——INT 13H)读扇区都参数说明：
! 功能02H
!
! 功能描述：读扇区
!
! 入口参数：AH＝02H AL＝扇区数 CH＝柱面 CL＝扇区 DH＝磁头
! DL＝驱动器，00H~7FH：软盘；80H~0FFH：硬盘
! ES:BX＝缓冲区的地址
!
! 出口参数：CF＝0——操作成功，AH＝00H，AL＝传输的扇区数，否则，AH＝状态代码，参见功能号01H中的说明

SYSSIZE	=	0x3000	! system模块大小
.global begtext, begdata, begbss, endtext, enddata, endbss
.text
begtext:
.data
begdata:
.bss
begbss:
.text
BOOTSEG	=	0X07c0 ! BIOS加载boot代码到指定的内存地址（原始段地址）（31k） 0x7c0是boot所在的段值，或者说单位是“节”（16字节）。换算成基地址值需要乘16，即0x7c00
! 总共有20根地址线也就是20位寻址.
! 0x07c0 是表示高16位地址位.
! 基地址是等于0x07c0加上四位二进制的位也就是一个十六进制的值=0x07c00
INITSEG	=	0x9000 ;将bootsec移动到这里
SYSSEG	=	0x1000	! system模块加载到内存0x10000（64K）处
SETUPLEN	=	4	! setup程序的扇区数值
ENDSEG	=	SYSSEG + SYSSIZE	! 停止加载system的段地址
SETUPSEG	=	0x9020
entry start !告知链接程序，程序从start处开始执行
ROOT_DEV	=	0x301	! 根文件系统设备号，暂时按照linux 0.11格式定义，后续将和linux 2.6内核一致
start:
	mov	ax,#BOOTSEG
	mov	ds,ax
	mov	ax,#INITSEG
	mov	es,ax
	mov	cx,#256	! 每次移动2个字节，引导扇区一共512个字节，所以移动256次
	sub	si,si
	sub	di,di
	rep	! 重复执行后面一条指令，并递减cx,将bootsec从ds:si=0x7c00:0000移动到es:di=0x9000:0000处，即0x90000
	movw
	jmpi	go,#INITSEG	! 段间跳转
	
go:	mov	ax,cs
	mov	ds,ax
	mov	es,ax
	mov	ss,ax
	mov	sp,#0xFF00	! 将堆栈指针指向内存0x9000:0xff00处，即0x9ff00处

load_setup:
	mov	dx,#0x0000 ! DH磁头号，DL驱动器号。
	mov	cx,#0x0002	!CH磁道号（共10位）高八位，CL6，7未为磁道号低两位。CL0-5为起始扇区号。（从1开始，setup在bootsec扇区之后，所以为2）
	mov	bx,#0x200	! ES：BX 读入缓冲区位置为,0X9000:0x0200即0x90200处
	mov	ax,#0X200+SETUPLEN ! 入口参数：AH＝02H AL＝扇区数
	int	0x13
	jnc	ok_load_setup ! jnc是一条跳转指令，当进位标记C为0时跳转，为1时执行后面的指令。
	mov	dx,#0x0000
	mov	ax,#0x0000
	int	0x13	! 磁盘系统复位
	j	load_setup

ok_load_setup:
! 功能08H

! 功能描述：读取驱动器参数

! 入口参数：AH＝08H 
! DL＝驱动器，00H~7FH：软盘；80H~0FFH：硬盘
! 
! 出口参数：CF＝1——操作失败，AH＝状态代码，参见功能号01H中的说明，否则， BL＝01H — 360K 
! ＝02H — 1.2M 
! ＝03H — 720K 
! ＝04H — 1.44M
! 
! CH＝柱面（磁道）数的低8位 
! CL的位7-6＝柱面数的高2位 
! CL的位5-0＝每磁道扇区数 
! DH＝磁头数 
! DL＝驱动器数 
! ES:DI＝磁盘驱动器参数表地址
	mov	dl,#0x00
	mov	ax,#0x0800	! ah=8 读取驱动器参数
	int	0x13
	mov	ch,#00
	seg	cs	! 影响下一条语句操作数在cs指定的段中偏移
	mov	sectors,cx	!软盘最大磁道不会超过256，所以ch已经够用，cl都6，7位肯定为0.所以此时cx就是每磁道扇区数
	mov	ax,#INITSEG
	mov	es,ax	! 把cs改回来

	mov	ah,#0x03
	xor	bh,bh
	int	0x10	! 获取光标位置，dh行，dl列
	
	mov	cx,#32
	mov	bx,#0x0007
	mov	bp,#msg1
	mov	ax,#0x1301	! 显示字符，光标跟随移动
	int	0x10

	mov	ax,#SYSSEG
	mov	es,ax	! es=存放system的段地址
	call	read_it	! 读磁盘上都system模块
	call	kill_motor	! 关闭驱动器马达

! 设备号=主设备号*256+次设备号
! 软驱的次设备号=type*4+nr	nr为0-3分别代表A,B,C,D盘,type 2为1.2M	7为1.44M
	seg	cs
	mov	ax,root_dev
	cmp	ax,#0
	jne	root_defined

	seg	cs
	mov	bx,sectors
	mov	ax,#0x208
	cmp	bx,#15
	je	root_defined
	mov	ax,#0x21c
	cmp	bx,#18
	je	root_defined
undef_root:
	jmp	undef_root

root_defined:
	seg	cs
	mov	root_dev,ax

jmpi	0,SETUPSEG

sread:	.word	1+SETUPLEN	! 当前磁道已读扇区数，boot为1+setup=4
head:	.word	0	! 当前磁头号
track:	.word	0	! 当前磁道号
read_it:
	mov	ax,es
	test	ax,#0x0fff
die:	jne	die	! 零标志位为0，证明es不等于0x1000，则进入死循环
	xor	bx,bx
rp_read:
	mov	ax,es
	cmp	ax,#ENDSEG
	jb	ok1_read	! 小于则跳转
	ret
ok1_read:
	seg	cs
	mov	ax,sectors	! sectors中为每磁道扇区数
	sub	ax,sread	! 减去当前磁道已读扇区数
	mov	cx,ax
	shl	cx,#9
	add	cx,bx		! cx=cx*512字节+段内当前偏移bx，此时cx中保存为当前段内总共读入都字节数
	jnc	ok2_read	! cx中的值没有超过64K则跳转
	je	ok2_read	! 运算结果为0（zf=1）则跳转,此时ax,bx都为0，证明是新磁道
	! 如果超过64K	
	xor	ax,ax
	sub	ax,bx
	shr	ax,#9	! bx为本段可以读取都最大字节，ax为当前磁道可以读取都最大扇区数
ok2_read:
	call	read_track	! 读取当前磁道上指定开始扇区和需读取扇区的数据
	mov	cx,ax	! cx为本次操作已经读入的扇区数
	add	ax,sread	! ax中为当前磁道已读扇区数
	seg	cs
	cmp	ax,sectors
	jne	ok3_read
	! 若该磁道已经读完，则读取该磁道下一磁头面都磁道，如果已经完成，则读取下一磁道
	mov	ax,#1
	sub	ax,head
	jne	ok4_read	! 如果是0磁头，则读取1磁头上面的扇区数据
	inc	track	! 否则去读取下一磁道
ok4_read:
	mov	head,ax	! 保存当前磁头号
	xor	ax,ax	! 清当前磁道已读扇区
ok3_read:
	mov	sread,ax
	shl	cx,#9
	add	bx,cx	! 调整当前段内数据开始位置
	jnc	rp_read	! 若没有超过段界限，则继续读取到本段内
	mov	ax,es
	add	ax,#0x1000 ! 将段基址指向下一64K偏移处
	mov	es,ax	! 改变段值
	xor	bx,bx
	jmp	rp_read

! 将al个扇区读入到es:bx处，开始为0x1000:0
read_track:
	push	ax
	push	bx
	push	cx
	push	dx
	mov	dx,track	! 取得当前磁道号
	mov	cx,sread	! 取得当前磁道上已读扇区数
	inc	cx	! cl当前准备读的扇区
	mov	ch,dl	! 当前磁道号
	mov	dx,head ! 当前磁头号
	mov	dh,dl
	mov	dl,#0	! 驱动器为软盘
	and	dx,#0x0100 ! 磁头号不大于1
	mov	ah,#2	! 读扇区	al中有当前磁道未读取都扇区数
	int	0x13
	jc	bad_rt	! 操作失败cf=1则跳转
	pop	dx
	pop	cx
	pop	bx
	pop	ax
	ret
! 若读取失败，则复位后重新读取
bad_rt:	mov	ax,#0
	mov	dx,#0
	int	0x13
	pop	dx
	pop	cx
	pop	bx
	pop	ax
	jmp	read_track

kill_motor:
	push	dx
	mov	dx,#0x3f2	! 软盘控制器端口
	mov	al,#0
	outb
	pop	dx
	ret

sectors:
	.word	0
msg1:
	.byte	13,10	! 回车，换行
	.ascii	"Play OS Loading system ..."
	.byte	13,10,13,10
.org	508
root_dev:
	.word	ROOT_DEV
boot_flag:
	.word	0xAA55
.text
endtext:
.data
enddata:
.bss
endbss:
