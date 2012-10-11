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
SETUPLEN = 4	! setup.bin最大占用4个扇区
entry start !告知链接程序，程序从start处开始执行
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
	mov	bx,#0x200	! EX：BX 读入缓冲区位置为,0X9000:0x0200即0x90200处
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
	seg	cs	! 影响下一条语句操作数在cs指定都段中偏移
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
	

sectors:
	.word	0
msg1:
	.byte	13,10	! 回车，换行
	.ascii	"Play OS Loading system ..."
	.byte	13,10,13,10
.org	510
	.word	0xAA55
.text
endtext:
.data
enddata:
.bss
endbss:
