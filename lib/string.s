#
# 本程序功能为:字符打印函数
# author:ke.chenk
# mail:lingqi1818@gmail.com
#
.global write_char
.text
write_char:
	mov	4(%esp),%ax
	push	%gs
	pushl	%ebx
	movl	$0x18,%ebx
	mov	%bx,%gs
	movl	scr_loc,%ebx
	shl	$1,%ebx # 屏幕中每个字符占2个字节，其中一个为属性字节
	movb	$0x0f,%ah # 0000: 黑底    1111: 白字
	mov	%ax,%gs:(%ebx)
	shr	$1,%ebx
	incl	%ebx
	cmpl	$2000,%ebx
	jb	1f
	movl	$0,%ebx
1:	movl	%ebx,scr_loc
	popl	%ebx
	pop	%gs
	ret

scr_loc:.long 0 #屏幕当前位置
