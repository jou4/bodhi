extern print_number
extern exit
section .text
global _main
_main:
	push rbp
	mov rbp, rsp
	sub rsp, 64
	mov	rax, 1
	mov	rbx, 2
	mov	rcx, 3
	mov	qword [rbp - 0], rcx
	mov	rcx, rax
	add	rcx, rbx
	mov	qword [rbp - 8], rax
	mov	qword [rbp - 16], rbx
	mov	rbx, [rbp - 0]
	mov	rax, rcx
	add	rax, rbx
	mov	rbx, [rbp - 8]
	mov	rcx, rax
	add	rcx, rbx
	mov	rax, [rbp - 16]
	mov	rbx, rcx
	add	rbx, rax
	mov	rax, [rbp - 0]
	add	rbx, rax
	mov	[rsp], rbx
	call print_number
	call exit
