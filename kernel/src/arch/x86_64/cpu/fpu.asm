; Taken from: https://github.com/purpleK2/kernel/blob/e6ff7cdeb50169a6715c6e6c0919a46f6f7b4245/src/arch/x86_64/math/fpu.c#L7

default rel

section .text
global fpu_enable
fpu_enable:
    mov rax, cr0
    and rax, ~(1 << 2)  ; clear EM: disable emulation
    or rax,   (1 << 1)  ; set MP: monitor FPU
    or rax,   (1 << 5)  ; set NE: enable internal x87 error reporting
    mov cr0, rax
    fninit
    ret

