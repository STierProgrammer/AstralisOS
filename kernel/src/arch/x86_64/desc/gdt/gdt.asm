[BITS 64]
default rel

section .text
global reload_gdt

reload_gdt:
    lea rax, [rel .reload_cs]
    push 0x08
    push rax
    retfq

.reload_cs:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax
    xor ax, ax
    mov fs, ax
    mov gs, ax
    ret

