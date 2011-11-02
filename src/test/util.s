extern _printf

section .data
    br              db      0x0a, 0
    number_format   db      "%d", 0x0a, 0

section .text
global strlen
global print
global println
global print_number
global puts
global exit


strlen:
    push rbp
    mov rbp, rsp
    sub rsp, 16
    mov qword [rbp - 8], 0
strlen_cmp:
    cmp byte [rax], 0
    je strlen_end
    inc rax
    inc qword [rbp - 8]
    jmp strlen_cmp
strlen_end:
    mov rax, [rbp - 8]  ; return
    mov rsp, rbp
    pop rbp
    ret


print:
    push rbp
    mov rbp, rsp
    sub rsp, 32
print_1:
    ; get length of string
    mov rax, [rbp + 16]
    mov [rsp], rax
    call strlen
    mov [rbp - 8], rax
    ; print string
    mov rax, [rbp + 16]
    mov [rsp], rax
    mov rax, [rbp - 8]
    mov [rsp + 8], rax
    call puts
print_2:
    mov rsp, rbp
    pop rbp
    ret


println:
    push rbp
    mov rbp, rsp
    sub rsp, 16
println_1:
    ; puts string
    mov rax, [rbp + 16]
    mov [rsp], rax
    call print
    ; puts breakline
    mov rax, br
    mov qword [rsp], rax
    call print
println_2:
    mov rsp, rbp
    pop rbp
    ret


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


puts:
    push rbp
    mov rbp, rsp
    sub rsp, 16
puts_1:
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi
puts_2:
    ; print string
    mov rax, 0x2000004      ; System call write = 4
    mov rdi, 1              ; Write to standard out = 1
    mov rsi, [rbp + 16]     ; The address of string
    mov rdx, [rbp + 24]     ; The size to write
    syscall                 ; Invoke the kernel
puts_3:
    mov rdi, [rbp - 8]
    mov rsi, [rbp - 16]
puts_4:
    mov rsp, rbp
    pop rbp
    ret


exit:
    mov rax, 0x2000001      ; System call number for exit = 1
    mov rdi, 0              ; Exit success = 0
    syscall                 ; Invoke the kernel
