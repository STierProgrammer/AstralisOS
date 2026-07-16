[BITS 64]

start: 
    mov rax, 0
    lea rdi, [rel sigma]
    int 0x80
    jmp $

sigma:
    db "sigma.txt", 0


