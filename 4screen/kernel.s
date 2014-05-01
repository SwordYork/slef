	.file	"kernel.c"
	.text
	.globl	main
	.type	main, @function
main:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	call	init_palette
	movq	$655360, -16(%rbp)
	movl	$0, -4(%rbp)
	jmp	.L2
.L3:
	movl	-4(%rbp), %eax
	movslq	%eax, %rdx
	movq	-16(%rbp), %rax
	addq	%rdx, %rax
	movl	-4(%rbp), %edx
	andl	$15, %edx
	movb	%dl, (%rax)
	addl	$1, -4(%rbp)
.L2:
	cmpl	$65535, -4(%rbp)
	jle	.L3
.L4:
	call	_io_hlt
	jmp	.L4
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.globl	init_palette
	.type	init_palette, @function
init_palette:
.LFB1:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	$table_rgb.1301, %edx
	movl	$15, %esi
	movl	$0, %edi
	call	set_palette
	nop
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1:
	.size	init_palette, .-init_palette
	.globl	set_palette
	.type	set_palette, @function
set_palette:
.LFB2:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movl	%edi, -20(%rbp)
	movl	%esi, -24(%rbp)
	movq	%rdx, -32(%rbp)
	call	_io_load_eflags
	movl	%eax, -8(%rbp)
	call	_io_cli
	movl	-20(%rbp), %eax
	movl	%eax, %esi
	movl	$968, %edi
	call	_io_out8
	movl	-20(%rbp), %eax
	movl	%eax, -4(%rbp)
	jmp	.L8
.L9:
	movq	-32(%rbp), %rax
	movzbl	(%rax), %eax
	shrb	$2, %al
	movzbl	%al, %eax
	movl	%eax, %esi
	movl	$969, %edi
	call	_io_out8
	movq	-32(%rbp), %rax
	addq	$1, %rax
	movzbl	(%rax), %eax
	shrb	$2, %al
	movzbl	%al, %eax
	movl	%eax, %esi
	movl	$969, %edi
	call	_io_out8
	movq	-32(%rbp), %rax
	addq	$2, %rax
	movzbl	(%rax), %eax
	shrb	$2, %al
	movzbl	%al, %eax
	movl	%eax, %esi
	movl	$969, %edi
	call	_io_out8
	addq	$3, -32(%rbp)
	addl	$1, -4(%rbp)
.L8:
	movl	-4(%rbp), %eax
	cmpl	-24(%rbp), %eax
	jle	.L9
	movl	-8(%rbp), %eax
	movl	%eax, %edi
	call	_io_store_eflags
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE2:
	.size	set_palette, .-set_palette
	.data
	.align 32
	.type	table_rgb.1301, @object
	.size	table_rgb.1301, 48
table_rgb.1301:
	.byte	0
	.byte	0
	.byte	0
	.byte	-1
	.byte	0
	.byte	0
	.byte	0
	.byte	-1
	.byte	0
	.byte	-1
	.byte	-1
	.byte	0
	.byte	0
	.byte	0
	.byte	-1
	.byte	-1
	.byte	0
	.byte	-1
	.byte	0
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	-58
	.byte	-58
	.byte	-58
	.byte	-124
	.byte	0
	.byte	0
	.byte	0
	.byte	-124
	.byte	0
	.byte	-124
	.byte	-124
	.byte	0
	.byte	0
	.byte	0
	.byte	-124
	.byte	-124
	.byte	0
	.byte	-124
	.byte	0
	.byte	-124
	.byte	-124
	.byte	-124
	.byte	-124
	.byte	-124
	.ident	"GCC: (GNU) 4.8.2 20140206 (prerelease)"
	.section	.note.GNU-stack,"",@progbits
