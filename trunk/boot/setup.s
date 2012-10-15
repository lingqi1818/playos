!
! 利用BIOS中断，读取机器系统数据，存入0x90000开始都内存位置处，也就是覆盖bootsect
! 然后将system模块从0x10000位置挪到0x00000处
! author:ke.chenk             		            
! mail:lingqi1818@gmail.com
!
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
	mov	ax,cs
	mov	ds,ax
	mov	es,ax

	mov	ah,#0x03
	xor	bh,bh
	int	0x10	! 获取光标位置，dh行，dl列

	mov	cx,#26
	mov	bx,#0x0007
	mov	bp,#test_msg
	mov	ax,#0x1301	! 显示字符，光标跟随移动
	int	0x10

test_msg:
	.byte	13,10	! 回车，换行
	.ascii	"in setup process ..."
	.byte	13,10,13,10
.text
endtext:
.data
enddata:
.bss
endbss:
