# 
# 本程序功能为：内核system模块的头部代码，设置gdt,idt，校验A20线是否有效，跳入main函数
# author:ke.chenk             		            
# mail:lingqi1818@gmail.com
#
# 注意：此时程序已经进入保护模式，所以寻址方式为：段选择子+偏移
.text
.globl idt,gdt
pg_dir:	#页目录将会存放在这里，也就是地址0x00000
startup_32:
	movl	$0x10,%eax	# 数据段选择子
	mov	%ax,%ds		# 非常重要，设置错误无法加载gdt
	mov	%ax,%es
	mov	%ax,%fs
	mov	%ax,%gs
	lss	_stack_start,%esp

	call	setup_idt
	call	setup_gdt
	
	movl	$0x10,%eax
	mov	%ax,%ds
	mov	%ax,%es
	mov	%ax,%fs
	mov	%ax,%gs
	lss	_stack_start,%esp
	
	xorl	%eax,%eax
1:	incl	%eax
	movl	%eax,0x000000
	cmpl	%eax,0x100000
	je	1b

	movl	%cr0,%eax
	#save pg位31分页标志位，pe位0设置保护标志位，ET：CR0的位4是扩展类型（Extension Type）标志。当该标志为1时，表示指明系统中有80387协处理器，并使用32位协处理器协议。ET=0指明使用80287协处理器。如果仿真位EM=1，则该位将被忽略。在处理器复位操作时，ET位会被初始化指明系统中使用的协处理器类型。如果系统中有80387，则ET被设置成1，否则若有一个80287或者没有协处理器，则ET被设置成0。
	#EM：CR0的位2是仿真（EMulation）标志。当该位设置时，表示处理器没有内部或外部协处理器，执行协处理器指令时会引起设备不存在异常；当清除时，表示系统有协处理器。设置这个标志可以迫使所有浮点指令使用软件来模拟。
	#MP：CR0的位1是监控协处理器（Monitor coProcessor或Math Present）标志。用于控制WAIT/FWAIT指令与TS标志的交互作用。如果MP=1、TS=1，那么执行WAIT指令将产生一个设备不存在异常；如果MP=0，则TS标志不会影响WAIT的执行。
	andl	$0x80000011,%eax
	orl	$2,%eax	#set mp
	movl	%eax,%cr0
	call	check_x87
	jmp	after_page_tables

check_x87:
	fninit	#初始化协处理器命令
	fstsw	%ax	#取协处理器状态字
	cmpb	$0,%al	#初始化后，状态字应该为0
	je	1f
	movl	%cr0,%eax
	xorl	$6,%eax	#reset mp,em
	movl	%eax,%cr0
	ret

1:	.byte	0xdb,0xe4	#80287设置保护模式fsetpm,80387自动忽略
	ret

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

.org	0x1000
pg0:
.org	0x2000
pg1:
.org	0x3000
pg2:
.org	0x4000
pg3:
.org	0x5000
after_page_tables:
	pushl	$0
	pushl	$0
	pushl	$0
	pushl	$L6
	pushl	$main
	jmp	setup_paging
L6:
	jmp	L6


setup_paging:
	ret

idt_descr:
	.word	256*8-1
	.long	idt

.word	0

gdt_descr:
	.word	256*8-1
	.long	gdt

idt:	.fill	256,8,0
gdt:	.quad 0x0000000000000000
	.quad 0x00c09a0000000fff	#代码段
	.quad 0x00c0920000000fff	#数据段
	.quad 0x00c0920b80000002	#显存段，界限为2*4k
	.fill	252,8,0

_stack_start:		#注意，汇编的堆栈是高地址向低地址伸展，所以，需要在之前开辟空闲都空间
	.long _stack_start	#堆栈段偏移位置
	.word 0x10		#堆栈段同内核数据段

