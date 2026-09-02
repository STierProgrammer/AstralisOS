default rel

section .text

%macro pushaq 0
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
%endmacro

%macro popaq 0
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
%endmacro

global isr_divide_error
isr_divide_error:
    push 0
    push 0x00
    jmp isr_exception

global isr_debug_exception
isr_debug_exception:
    push 0
    push 0x01
    jmp isr_exception

global isr_nmi_interrupt
isr_nmi_interrupt:
    push 0
    push 0x02
    jmp isr_exception

global isr_breakpoint
isr_breakpoint:
    push 0
    push 0x03
    jmp isr_exception

global isr_overflow
isr_overflow:
    push 0
    push 0x04
    jmp isr_exception

global isr_bound_range_exceeded
isr_bound_range_exceeded:
    push 0
    push 0x05
    jmp isr_exception

global isr_invalid_opcode
isr_invalid_opcode:
    push 0
    push 0x06
    jmp isr_exception

global isr_device_not_avaliable
isr_device_not_avaliable:
    push 0
    push 0x07
    jmp isr_exception

global isr_double_fault
isr_double_fault:
    push 0
    push 0x08
    jmp isr_exception

global isr_coprocessor_segment_overrun
isr_coprocessor_segment_overrun:
    push 0
    push 0x09
    jmp isr_exception

global isr_invalid_tss
isr_invalid_tss:
    push 0x0A
    jmp isr_exception

global isr_segment_not_present
isr_segment_not_present:
    push 0x0B
    jmp isr_exception

global isr_stack_segment_fault
isr_stack_segment_fault:
    push 0x0C
    jmp isr_exception

global isr_general_protection_fault
isr_general_protection_fault:
    push 0x0D
    jmp isr_exception

global isr_page_fault
isr_page_fault:
    push 0x0E
    jmp isr_exception

global isr_floating_point_error
isr_floating_point_error:
    push 0
    push 0x10
    jmp isr_exception

global isr_alignment_check
isr_alignment_check:
    push 0x11
    jmp isr_exception

global isr_machine_check
isr_machine_check:
    push 0
    push 0x12
    jmp isr_exception

global isr_simd_floating_point_exception
isr_simd_floating_point_exception:
    push 0
    push 0x13
    jmp isr_exception

global isr_virtualization_exception
isr_virtualization_exception:
    push 0
    push 0x14
    jmp isr_exception

global isr_control_protection_exception
isr_control_protection_exception:
    push 0x15
    jmp isr_exception
   
global isr_exception
extern isr_exception_handler
isr_exception:
   pushaq
   mov rdi, rsp
   cld
   call isr_exception_handler
   popaq
   add rsp, 16
   iretq

global isr_timer_interrupt
isr_timer_interrupt:
    push 0x20
    jmp isr_interrupt

global isr_keyboard_interrupt
isr_keyboard_interrupt:
    push 0x21
    jmp isr_interrupt

global isr_mouse_interrupt
isr_mouse_interrupt:
    push 0x2C
    jmp isr_interrupt

global isr_com1_interrupt
isr_com1_interrupt:
    push 0x30
    jmp isr_interrupt

global isr_syscall_interrupt
isr_syscall_interrupt:
    push 0x80
    jmp isr_interrupt

global isr_interrupt
extern isr_interrupt_handler
isr_interrupt:
    pushaq
    mov rdi, rsp
    cld
    call isr_interrupt_handler
    popaq
    add rsp, 8
    iretq

