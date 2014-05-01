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
	subq	$32, %rsp
	call	init_palette
	movq	$655360, -8(%rbp)
	movl	$320, -12(%rbp)
	movl	$200, -16(%rbp)
	movl	-16(%rbp), %eax
	leal	-29(%rax), %edx
	movl	-12(%rbp), %eax
	leal	-1(%rax), %ecx
	movl	-12(%rbp), %esi
	movq	-8(%rbp), %rax
	movl	%edx, (%rsp)
	movl	%ecx, %r9d
	movl	$0, %r8d
	movl	$0, %ecx
	movl	$14, %edx
	movq	%rax, %rdi
	call	boxfill8
	movl	-16(%rbp), %eax
	leal	-28(%rax), %edx
	movl	-12(%rbp), %eax
	leal	-1(%rax), %edi
	movl	-16(%rbp), %eax
	leal	-28(%rax), %ecx
	movl	-12(%rbp), %esi
	movq	-8(%rbp), %rax
	movl	%edx, (%rsp)
	movl	%edi, %r9d
	movl	%ecx, %r8d
	movl	$0, %ecx
	movl	$8, %edx
	movq	%rax, %rdi
	call	boxfill8
	movl	-16(%rbp), %eax
	leal	-27(%rax), %edx
	movl	-12(%rbp), %eax
	leal	-1(%rax), %edi
	movl	-16(%rbp), %eax
	leal	-27(%rax), %ecx
	movl	-12(%rbp), %esi
	movq	-8(%rbp), %rax
	movl	%edx, (%rsp)
	movl	%edi, %r9d
	movl	%ecx, %r8d
	movl	$0, %ecx
	movl	$7, %edx
	movq	%rax, %rdi
	call	boxfill8
	movl	-16(%rbp), %eax
	leal	-1(%rax), %edx
	movl	-12(%rbp), %eax
	leal	-1(%rax), %edi
	movl	-16(%rbp), %eax
	leal	-26(%rax), %ecx
	movl	-12(%rbp), %esi
	movq	-8(%rbp), %rax
	movl	%edx, (%rsp)
	movl	%edi, %r9d
	movl	%ecx, %r8d
	movl	$0, %ecx
	movl	$8, %edx
	movq	%rax, %rdi
	call	boxfill8
	movl	-16(%rbp), %eax
	leal	-24(%rax), %edx
	movl	-16(%rbp), %eax
	leal	-24(%rax), %ecx
	movl	-12(%rbp), %esi
	movq	-8(%rbp), %rax
	movl	%edx, (%rsp)
	movl	$59, %r9d
	movl	%ecx, %r8d
	movl	$3, %ecx
	movl	$7, %edx
	movq	%rax, %rdi
	call	boxfill8
	movl	-16(%rbp), %eax
	leal	-4(%rax), %edx
	movl	-16(%rbp), %eax
	leal	-24(%rax), %ecx
	movl	-12(%rbp), %esi
	movq	-8(%rbp), %rax
	movl	%edx, (%rsp)
	movl	$2, %r9d
	movl	%ecx, %r8d
	movl	$2, %ecx
	movl	$7, %edx
	movq	%rax, %rdi
	call	boxfill8
	movl	-16(%rbp), %eax
	leal	-4(%rax), %edx
	movl	-16(%rbp), %eax
	leal	-4(%rax), %ecx
	movl	-12(%rbp), %esi
	movq	-8(%rbp), %rax
	movl	%edx, (%rsp)
	movl	$59, %r9d
	movl	%ecx, %r8d
	movl	$3, %ecx
	movl	$15, %edx
	movq	%rax, %rdi
	call	boxfill8
	movl	-16(%rbp), %eax
	leal	-5(%rax), %edx
	movl	-16(%rbp), %eax
	leal	-23(%rax), %ecx
	movl	-12(%rbp), %esi
	movq	-8(%rbp), %rax
	movl	%edx, (%rsp)
	movl	$59, %r9d
	movl	%ecx, %r8d
	movl	$59, %ecx
	movl	$15, %edx
	movq	%rax, %rdi
	call	boxfill8
	movl	-16(%rbp), %eax
	leal	-3(%rax), %edx
	movl	-16(%rbp), %eax
	leal	-3(%rax), %ecx
	movl	-12(%rbp), %esi
	movq	-8(%rbp), %rax
	movl	%edx, (%rsp)
	movl	$59, %r9d
	movl	%ecx, %r8d
	movl	$2, %ecx
	movl	$0, %edx
	movq	%rax, %rdi
	call	boxfill8
	movl	-16(%rbp), %eax
	leal	-3(%rax), %edx
	movl	-16(%rbp), %eax
	leal	-24(%rax), %ecx
	movl	-12(%rbp), %esi
	movq	-8(%rbp), %rax
	movl	%edx, (%rsp)
	movl	$60, %r9d
	movl	%ecx, %r8d
	movl	$60, %ecx
	movl	$0, %edx
	movq	%rax, %rdi
	call	boxfill8
	movl	-16(%rbp), %eax
	leal	-24(%rax), %ecx
	movl	-12(%rbp), %eax
	leal	-4(%rax), %r8d
	movl	-16(%rbp), %eax
	leal	-24(%rax), %edi
	movl	-12(%rbp), %eax
	leal	-47(%rax), %edx
	movl	-12(%rbp), %esi
	movq	-8(%rbp), %rax
	movl	%ecx, (%rsp)
	movl	%r8d, %r9d
	movl	%edi, %r8d
	movl	%edx, %ecx
	movl	$15, %edx
	movq	%rax, %rdi
	call	boxfill8
	movl	-16(%rbp), %eax
	leal	-4(%rax), %ecx
	movl	-12(%rbp), %eax
	leal	-47(%rax), %r8d
	movl	-16(%rbp), %eax
	leal	-23(%rax), %edi
	movl	-12(%rbp), %eax
	leal	-47(%rax), %edx
	movl	-12(%rbp), %esi
	movq	-8(%rbp), %rax
	movl	%ecx, (%rsp)
	movl	%r8d, %r9d
	movl	%edi, %r8d
	movl	%edx, %ecx
	movl	$15, %edx
	movq	%rax, %rdi
	call	boxfill8
	movl	-16(%rbp), %eax
	leal	-3(%rax), %ecx
	movl	-12(%rbp), %eax
	leal	-4(%rax), %r8d
	movl	-16(%rbp), %eax
	leal	-3(%rax), %edi
	movl	-12(%rbp), %eax
	leal	-47(%rax), %edx
	movl	-12(%rbp), %esi
	movq	-8(%rbp), %rax
	movl	%ecx, (%rsp)
	movl	%r8d, %r9d
	movl	%edi, %r8d
	movl	%edx, %ecx
	movl	$7, %edx
	movq	%rax, %rdi
	call	boxfill8
	movl	-16(%rbp), %eax
	leal	-3(%rax), %ecx
	movl	-12(%rbp), %eax
	leal	-3(%rax), %r8d
	movl	-16(%rbp), %eax
	leal	-24(%rax), %edi
	movl	-12(%rbp), %eax
	leal	-3(%rax), %edx
	movl	-12(%rbp), %esi
	movq	-8(%rbp), %rax
	movl	%ecx, (%rsp)
	movl	%r8d, %r9d
	movl	%edi, %r8d
	movl	%edx, %ecx
	movl	$7, %edx
	movq	%rax, %rdi
	call	boxfill8
.L2:
	call	_io_hlt
	jmp	.L2
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
	movl	$table_rgb.1307, %edx
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
	jmp	.L6
.L7:
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
.L6:
	movl	-4(%rbp), %eax
	cmpl	-24(%rbp), %eax
	jle	.L7
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
	.globl	boxfill8
	.type	boxfill8, @function
boxfill8:
.LFB3:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	%rdi, -24(%rbp)
	movl	%esi, -28(%rbp)
	movl	%edx, %eax
	movl	%ecx, -36(%rbp)
	movl	%r8d, -40(%rbp)
	movl	%r9d, -44(%rbp)
	movb	%al, -32(%rbp)
	movl	-40(%rbp), %eax
	movl	%eax, -8(%rbp)
	jmp	.L10
.L13:
	movl	-36(%rbp), %eax
	movl	%eax, -4(%rbp)
	jmp	.L11
.L12:
	movl	-8(%rbp), %eax
	imull	-28(%rbp), %eax
	movl	%eax, %edx
	movl	-4(%rbp), %eax
	addl	%edx, %eax
	movslq	%eax, %rdx
	movq	-24(%rbp), %rax
	addq	%rax, %rdx
	movzbl	-32(%rbp), %eax
	movb	%al, (%rdx)
	addl	$1, -4(%rbp)
.L11:
	movl	-4(%rbp), %eax
	cmpl	-44(%rbp), %eax
	jle	.L12
	addl	$1, -8(%rbp)
.L10:
	movl	-8(%rbp), %eax
	cmpl	16(%rbp), %eax
	jle	.L13
	nop
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE3:
	.size	boxfill8, .-boxfill8
	.data
	.align 32
	.type	table_rgb.1307, @object
	.size	table_rgb.1307, 48
table_rgb.1307:
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
