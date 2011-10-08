extern _printf

section .data
    number_format   db      "%d", 0x0a, 0

section .text
global _main

_main:
    push rbp
    mov rbp, rsp

    sub rsp, 64

    ; body
    ; let a = 1 in let b = 2 in a + b
    ;mov    rax, 1
    ;mov    rbx, 2
    ;add    rax, rbx
    ;mov    [rsp], rax

    ; let a = 1 in let b = 2 in a + b + a + b
    ;mov    rax, 1
    ;mov    rbx, 2
    ;mov    rcx, rax
    ;add    rcx, rbx
    ;mov    qword [rbp - 0], rbx
    ;mov    rbx, rcx
    ;add    rbx, rax
    ;mov    rax, [rbp - 0]
    ;add    rbx, rax
    ;mov    [rsp], rbx

    ; let a = 1 in let b = 2 in let c = 3 in a + b + c
    mov    rax, 1
    mov    rbx, 2
    mov    rcx, 3
    mov    qword [rbp - 0], rcx
    mov    rcx, rax
    add    rcx, rbx
    mov    rax, [rbp - 0]
    add    rcx, rax
    mov    [rsp], rcx

    ; print
    call   print_number

    ; exit
    call exit


print_number:
    push rbp
    mov rbp, rsp
    sub rsp, 16
print_number_1:
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi
print_number_2:
    mov rsi, [rbp + 16]
    mov rdi, number_format
    mov rax, 0
    call _printf
print_number_3:
    mov rdi, [rbp - 8]
    mov rsi, [rbp - 16]
print_number_4:
    mov rsp, rbp
    pop rbp
    ret

exit:
    mov rax, 0x2000001      ; System call number for exit = 1
    mov rdi, 0              ; Exit success = 0
    syscall                 ; Invoke the kernel
