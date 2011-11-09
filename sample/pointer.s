	.text
.globl _make_closure
_make_closure:
LFB6:
	pushq	%rbp
LCFI0:
	movq	%rsp, %rbp
LCFI1:
	subq	$32, %rsp
LCFI2:
	movq	%rdi, -24(%rbp)
	movl	%esi, -28(%rbp)
	movl	-28(%rbp), %eax
	cltq
	leaq	8(%rax), %rdi
	call	_malloc
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rdx
	movq	-24(%rbp), %rax
	movq	%rax, (%rdx)
	movq	-8(%rbp), %rax
	leave
	ret
LFE6:
.globl _entry
_entry:
LFB7:
	pushq	%rbp
LCFI3:
	movq	%rsp, %rbp
LCFI4:
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	leave
	ret
LFE7:
.globl _freevars
_freevars:
LFB8:
	pushq	%rbp
LCFI5:
	movq	%rsp, %rbp
LCFI6:
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	addq	$8, %rax
	leave
	ret
LFE8:
	.cstring
LC0:
	.ascii "ABC\0"
	.text
.globl _main
_main:
LFB9:
	pushq	%rbp
LCFI7:
	movq	%rsp, %rbp
LCFI8:
	subq	$16, %rsp
LCFI9:
	movl	$96, %esi
	leaq	LC0(%rip), %rdi
	call	_make_closure
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rdi
	call	_entry
	movq	%rax, %rdi
	call	_puts
	movl	$0, %eax
	leave
	ret
LFE9:
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
.globl _make_closure.eh
_make_closure.eh:
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
.globl _entry.eh
_entry.eh:
LSFDE3:
	.set L$set$5,LEFDE3-LASFDE3
	.long L$set$5
LASFDE3:
	.long	LASFDE3-EH_frame1
	.quad	LFB7-.
	.set L$set$6,LFE7-LFB7
	.quad L$set$6
	.byte	0x0
	.byte	0x4
	.set L$set$7,LCFI3-LFB7
	.long L$set$7
	.byte	0xe
	.byte	0x10
	.byte	0x86
	.byte	0x2
	.byte	0x4
	.set L$set$8,LCFI4-LCFI3
	.long L$set$8
	.byte	0xd
	.byte	0x6
	.align 3
LEFDE3:
.globl _freevars.eh
_freevars.eh:
LSFDE5:
	.set L$set$9,LEFDE5-LASFDE5
	.long L$set$9
LASFDE5:
	.long	LASFDE5-EH_frame1
	.quad	LFB8-.
	.set L$set$10,LFE8-LFB8
	.quad L$set$10
	.byte	0x0
	.byte	0x4
	.set L$set$11,LCFI5-LFB8
	.long L$set$11
	.byte	0xe
	.byte	0x10
	.byte	0x86
	.byte	0x2
	.byte	0x4
	.set L$set$12,LCFI6-LCFI5
	.long L$set$12
	.byte	0xd
	.byte	0x6
	.align 3
LEFDE5:
.globl _main.eh
_main.eh:
LSFDE7:
	.set L$set$13,LEFDE7-LASFDE7
	.long L$set$13
LASFDE7:
	.long	LASFDE7-EH_frame1
	.quad	LFB9-.
	.set L$set$14,LFE9-LFB9
	.quad L$set$14
	.byte	0x0
	.byte	0x4
	.set L$set$15,LCFI7-LFB9
	.long L$set$15
	.byte	0xe
	.byte	0x10
	.byte	0x86
	.byte	0x2
	.byte	0x4
	.set L$set$16,LCFI8-LCFI7
	.long L$set$16
	.byte	0xd
	.byte	0x6
	.align 3
LEFDE7:
	.subsections_via_symbols
