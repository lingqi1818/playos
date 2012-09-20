!
!引导设备引导区代码，注意，本程序编译完之后生成的内容最多不能超过512K
!author:ke.chenk             		            
! mail:lingqi1818@gmail.com

.globl begtext, begdata, begbss, endtext, enddata, endbss !全局标识符，供ld86链接使用
.text ! 正文段
begtext:
.data ! 数据段
begdata:
.bss  ! 未初始化数据段
begbss:
.text  ! 正文段
BOOTSEG = 0X7C0 ！ BIOS加载boot代码到指定的内存地址（原始段地址）
entry start !告知链接程序，程序从start处开始执行
start:
	jmpi go,BOOTSEG ! 段间跳转，go是偏移地址
go:	mov ax,cs
	mov ds,ax
	mov es,ax
	mov [msg1+17],ah
	mov cx,#20 ! 共显示20个字符，包括回车换行
	mov dx,#0x1004 ! 字符串显示在17行，第5列
	mov bx,#0x000c ! 字符显示属性（红色）
	mov bp,#msg1 ! int 0x10需要指向显示的字符串
	mov ax,#0x1301! bios中断0x10,功能0x13,子功能01
	int 0x10
loop1:	jmp loop1 ! 死循环
msg1:	.ascii "playos Loading ..."
	.byte 13,10
.org	510
	.word 0xAA55
.text
endtext:
.data
enddata:
.bss
endbss:




