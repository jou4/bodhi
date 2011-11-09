	.cstring
LC0:
	.ascii "ABC\0"
LC1:
	.ascii "DEF\0"
	.text
.globl _main
_main:
LFB6:
	pushq	%rbp
LCFI0:
	movq	%rsp, %rbp
LCFI1:
	subq	$32, %rsp
LCFI2:
	leaq	LC0(%rip), %rax
	movq	%rax, -8(%rbp)
	leaq	LC1(%rip), %rax
	movq	%rax, -16(%rbp)
	movl	$16, %edi
	call	_malloc
	movq	%rax, -24(%rbp)
	movq	-24(%rbp), %rdx
	movq	-8(%rbp), %rax
	movq	%rax, (%rdx)
	movq	-24(%rbp), %rdx
	movq	-16(%rbp), %rax
	movq	%rax, 8(%rdx)
	movl	$16, %edi
	call	_malloc
	movq	%rax, -32(%rbp)
	movq	-8(%rbp), %rdx
	movq	-32(%rbp), %rax
	movq	%rdx, (%rax)
	movq	-32(%rbp), %rdx
	addq	$8, %rdx
	movq	-16(%rbp), %rax
	movq	%rax, (%rdx)
	movq	-24(%rbp), %rax
	movq	(%rax), %rdi
	call	_puts
	movq	-24(%rbp), %rax
	movq	8(%rax), %rdi
	call	_puts
	movq	-32(%rbp), %rax
	movq	(%rax), %rdi
	call	_puts
	movq	-32(%rbp), %rax
	movq	8(%rax), %rdi
	call	_puts
	movl	$0, %eax
	leave
	ret
LFE6:
	.section __TEXT,__eh_frame,coalesced,no_toc+strip_static_syms+live_support
EH_frame1:
	.set L$set$0,LECIE1-LSCIE1
	.long L$set$0
LSCIE1:
	.long	0x0
	.byte	0x1
	.ascii "zR\0"
	.byte	0x1
	.byte	0x78
	.byte	0x10
	.byte	0x1
	.byte	0x10
	.byte	0xc
	.byte	0x7
	.byte	0x8
	.byte	0x90
	.byte	0x1
	.align 3
LECIE1:
.globl _main.eh
_main.eh:
LSFDE1:
	.set L$set$1,LEFDE1-LASFDE1
	.long L$set$1
LASFDE1:
	.long	LASFDE1-EH_frame1
	.quad	LFB6-.
	.set L$set$2,LFE6-LFB6
	.quad L$set$2
	.byte	0x0
	.byte	0x4
	.set L$set$3,LCFI0-LFB6
	.long L$set$3
	.byte	0xe
	.byte	0x10
	.byte	0x86
	.byte	0x2
	.byte	0x4
	.set L$set$4,LCFI1-LCFI0
	.long L$set$4
	.byte	0xd
	.byte	0x6
	.align 3
LEFDE1:
	.subsections_via_symbols
