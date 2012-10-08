# 
# 本程序功能为：A，B进程在时钟中断都干扰下，轮流切换，并打印A和B
# author:ke.chenk             		            
# mail:lingqi1818@gmail.com
#
# 注意：此时程序已经进入保护模式，所以寻址方式为：段选择子+偏移
.data
SCRN_SEL	= 0x18 #显存选择子
LATCH		= 11930
.text
startup_32:
	mov	$0x10,%eax	# 数据段选择子
	mov	%ax,%ds		# 非常重要，设置错误无法加载gdt
	lss	init_stack,%esp
	call setup_idt
	call setup_gdt
	movl $0x10,%eax
	mov %ax,%ds
	mov %ax,%es
	mov %ax,%fs
	mov %ax,%gs
	lss init_stack,%esp
	#设置8253定时器
	movb	$0x36,%al	#通道0，工作方式为3
	movl	$0x43,%edx
	outb	%al,%dx
	movl	$LATCH,%eax	#1193180为8253的工作频率，这里需要每10毫秒响应一次，那么将数字设置为1193180/100
	movl	$0x40,%edx
	outb	%al,%dx
	movb	%ah,%al
	outb	%al,%dx
	sti
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


scr_loc:.long 0 #屏幕当前位置

ignore_int:
	push	%ds
	pushl	%eax
	movl	$0x10,%eax
	mov	%ax,%ds		#让ds指向内核数据段，因为中断程序属于内核
	movl	$67,%eax
	call	write_char
	popl	%eax
	pop	%ds
	iret

setup_gdt:
	lgdt lgdt_opcode
	ret

setup_idt:
	lea ignore_int,%edx	
	movl $0x00080000,%eax
	movw %dx,%ax		#eax为选择子（0x0008）+偏移
	movw $0x8E00,%dx	#中断门类型，特权级为0
	lea idt,%edi
	mov $256,%ecx
rp_sidt:movl %eax,(%edi)	#将eax中都值放入%edi中指向都地址
	movl %edx,4(%edi)	#将edx放入%edi+4指向都地址，这样一个中断门构造完毕
	addl $8,%edi		#将edi中的指针偏移8个字节
	dec	%ecx
	jne	rp_sidt
	lidt	lidt_opcode
	ret

lidt_opcode:
	.word (256*8)-1		#表长度
	.long idt		#基地址

lgdt_opcode:
	.word (end_gdt-gdt)-1
	.long gdt

idt:	.fill 256,8,0	#256个门描述符，每个8字节，门描述符和普通描述符类似，结构可以参考《自己动手编写操作系统》第三章

gdt:	.quad 0x0000000000000000
	.quad 0x00c09a00000007ff
	.quad 0x00c09200000007ff
	.quad 0x00c0920b80000002	#显存段，界限为2*4k

end_gdt:
	.fill 128,4,0

init_stack:		#注意，堆栈最好放在最后面，否则将会覆盖前面都指令，导致系统崩溃
	.long init_stack
	.word 0x10

