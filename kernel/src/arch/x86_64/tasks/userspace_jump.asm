[BITS 64]

default rel

global userspace_jump
extern user_test_entry

section .text
userspace_jump:
	mov rax, (4 * 8) | 3
	mov ds, ax
	mov es, ax 
	mov fs, ax 
	mov gs, ax
    
	mov rax, 0x510000
	push (4 * 8) | 3
	push rax
	pushf
	push (3 * 8) | 3
	push 0x400000
    iretq
