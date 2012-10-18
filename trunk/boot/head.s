# 
# 本程序功能为：内核system模块的头部代码，设置gdt,idt，校验A20线是否有效，跳入main函数
# author:ke.chenk             		            
# mail:lingqi1818@gmail.com
#
# 注意：此时程序已经进入保护模式，所以寻址方式为：段选择子+偏移
.text
_pg_dir:	#页目录将会存放在这里，也就是地址0x00000
startup_32:
	movl	$0x10,%eax	# 数据段选择子
	mov	%ax,%ds		# 非常重要，设置错误无法加载gdt
	mov	%ax,%es
	mov	%ax,%fs
	mov	%ax,%gs
	#lss	_stack_start,%esp
	call	setup_idt
	call	setup_gdt
	die:	jmp	die

setup_idt:
	lea	ignore_int,%edx
	movl	$0x00080000,%eax	#0x08代码段选择子
	movw	%dx,%ax
	movw	$0x8e00,%dx	#中断门，dpl=0,present
	lea	_idt,%edi
	mov	$256,%ecx
rp_sidt:
	movl	%eax,(%edi)
	movl	%edx,4(%edi)
	addl	$8,%edi
	dec	%ecx
	jne	rp_sidt
	lidt	idt_descr
	ret
	
setup_gdt:
	lgdt	gdt_descr
	ret

ignore_int:

idt_descr:
	.word	256*8-1
	.long	_idt

.word	0

gdt_descr:
	.word	256*8-1
	.long	_gdt

_idt:	.fill	256,8,0
_gdt:	.quad 0x0000000000000000
	.quad 0x00c09a0000000fff	#代码段
	.quad 0x00c0920000000fff	#数据段
	.quad 0x0000000000000000
	.fill	252,8,0
