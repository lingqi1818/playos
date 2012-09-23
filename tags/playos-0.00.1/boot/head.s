# 
# 简易内核程序，进入保护模式之后打印：play os
# author:ke.chenk             		            
# mail:lingqi1818@gmail.com
#
# 注意：此时程序已经进入保护模式，所以寻址方式为：段选择子+偏移
.data
SCRN_SEL	= 0x18 #显存选择子
.text
startup_32:
	mov	$0x10,%eax	# 数据段选择子
	mov	%ax,%ds		# 非常重要，设置错误无法加载gdt
	lss	init_stack,%esp
	call setup_gdt
	movl $0x10,%eax
	mov %ax,%ds
	mov %ax,%es
	mov %ax,%fs
	mov %ax,%gs
	lss init_stack,%esp
	movl $'P',%eax
	call write_char
	movl $'L',%eax
	call write_char
	movl $'A',%eax
	call write_char
	movl $'Y',%eax
	call write_char
	movl $' ',%eax
	call write_char
	movl $'O',%eax
	call write_char
	movl $'S',%eax
	call write_char
die:	jmp	die


write_char:
	push	%gs
	pushl	%ebx
	movl	$SCRN_SEL,%ebx
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


scr_loc:.long 1800 #屏幕当前位置


setup_gdt:
	lgdt lgdt_opcode
	ret


lgdt_opcode:
	.word (end_gdt-gdt)-1
	.long gdt


gdt:	.quad 0x0000000000000000
	.quad 0x00c09a00000007ff
	.quad 0x00c09200000007ff
	.quad 0x00c0920b80000002	#显存段，界限为2*4k

end_gdt:
	.fill 128,4,0

init_stack:		#注意，堆栈最好放在最后面，否则将会覆盖前面都指令，导致系统崩溃
	.long init_stack
	.word 0x10

