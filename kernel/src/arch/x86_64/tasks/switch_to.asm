[BITS 64]

default rel

TASK_RSP_OFFSET equ 16

extern task_switch

section .text
global switch_to
switch_to:
    push rbx
    push rbp
    push r12
    push r13
    push r14
    push r15
    
    mov [rdi + TASK_RSP_OFFSET], rsp
    mov rsp, [rsi + TASK_RSP_OFFSET]

    pop r15
    pop r14
    pop r13
    pop r12
    pop rbp
    pop rbx
    jmp task_switch


