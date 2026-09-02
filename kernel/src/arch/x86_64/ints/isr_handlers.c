#include "arch/x86_64/cpu/cpu.h"
#include "arch/x86_64/pit.h"
#include "misc/bitflags.h"
#include "misc/debug.h"
#include "tasks/syscall.h"
#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include <arch/x86_64/ints/isr_handlers.h>
#include <arch/x86_64/ints/pic.h>
#include <drvs/ps2/kbd/ps2_kbd.h>
#include <drvs/ps2/ps2_mouse.h>
#include <devs/serial.h>
#include <tasks/sched.h>

typedef enum exception_t {
    EXCEPTION_DIVISION_ERROR,
    EXCEPTION_DEBUG,
    EXCEPTION_NON_MASKABLE_INTERRUPT,
    EXCEPTION_BREAKPOINT,
    EXCEPTION_OVERFLOW,
    EXCEPTION_BOUND_RANGE_EXCEEDED
} exception_t;

static char *exception_code_to_str[] = {
    "Division Error",
    "Debug",
    "Non-maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
    "Invalid Opcode",
    "Device Not Available",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack-Segment Fault",
    "General Protection Fault",
    "Page Fault",
    [16] = "x87 Floating-Point Exception",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Control Protection Exception",
    [28] = "Hypervisor Injection Exception",
    "VMM Communication Exception",
    "Security Exception",
    NULL,
    NULL,
};

static inline void print_eframe(exception_frame_t *eframe)
{ 
    srprintf("\n");
    srprintf("----- %s -----\n", exception_code_to_str[eframe->exception_code]);
    srprintf(
        "r15: %x, r14: %x, r13: %x\nr12: %x, r11: %x, r10: %x\nr9:  %x, r8:  %x, rbp: %x\nrdi: %x, rsi: %x, rdx: %x\nrcx: %x, rbx: %x, rax: %x\nrcs: %x, cs:  %x, rflags: %x\nrsp: %x, rip:  %x\n\n",
        eframe->r15,
        eframe->r14,
        eframe->r13,
        eframe->r12,
        eframe->r11,
        eframe->r10,
        eframe->r9,
        eframe->r8,
        eframe->rbp,
        eframe->rdi,
        eframe->rsi,
        eframe->rdx,
        eframe->rcx,
        eframe->rbx,
        eframe->rax,
        eframe->ss,
        eframe->cs,
        eframe->rflags,
        eframe->rsp,
        eframe->rip);
}


static const char *pf_err_codes[] = {
    "PRESENT",
    "WRITE",
    "USER",
    "RESERVED",
    "INSTRUCTION FETCH",
    "PROTECTION KEY",
    "SHADOW STACK",
    "SOFTWARE GUARD EXTENSIONS"
};

void isr_exception_handler(exception_frame_t *eframe)
{        
    srprintf("\e[0;31m");
    print_eframe(eframe); 
    
    switch (eframe->exception_code)
    {
    case 0xE:
    {
        srprintf("Faulting adddress: %x\n", read_cr2());
        srprintf("Error code: ");
        srprintf("%b", eframe->error_code);
        srprintf("\n");
        break; 
    }
    case 0xD:
    {

        break;
    }
    default:
        break;
    }
    
    hcf();
}

typedef enum interrupt_t {
    PIT_INT = 0x20,
    KBD_INT,
    MOUSE_INT = 0x2C,
    COM1_INT = 0x30,
    SYSCALL_INT = 0x80
} interrupt_t;

void isr_interrupt_handler(interrupt_frame_t *iframe)
{
    switch (iframe->vector_number) {
    case PIT_INT:
    {
        krnlctx(interrupt_controller)->send_eoi(iframe->vector_number);
        timer_callback();
        sched_switch();       
        break;
    }
    case KBD_INT:
        ps2_keyboard_callback(); 
        break;
    case MOUSE_INT: 
        ps2_mouse_callback();
        break;
    case COM1_INT:
    {
        serial_com1_callback();
        break;
    }
    case SYSCALL_INT:
    {
        syscall_handler(iframe);   
        break;
    }
    }

    krnlctx(interrupt_controller)->send_eoi(iframe->vector_number);
}
