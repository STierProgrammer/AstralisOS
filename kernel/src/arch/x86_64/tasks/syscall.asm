default rel

section .text
global syscall_set_number
syscall_set_number:
    mov rax, rdi
    ret

