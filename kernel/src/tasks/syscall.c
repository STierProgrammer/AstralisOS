#include "misc/debug.h"
#include <tasks/syscall.h>
#include <tasks/sched.h>

enum
{
    SYS_OPEN = 0,
};

typedef int syscall_fn_t(unsigned long arg1, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5, unsigned long arg6);
#define SYSCALL(NR, func) [NR] = (syscall_fn_t*)(func)
#define NUM_SYSCALLS sizeof(syscall_table)/sizeof(*syscall_table)

int sys_open(const char *path)
{
    debug("Yo I was opened: %s", path);
    return task_open(sched_curr_task(), path);
}

syscall_fn_t *syscall_table[] =
{
    SYSCALL(SYS_OPEN, sys_open),
};

void syscall_handler(interrupt_frame_t *iframe)
{
    uint64_t syscall_number = iframe->rax;   
    
    if (syscall_number < NUM_SYSCALLS)
    {
        iframe->rax = syscall_table[syscall_number](iframe->rdi, iframe->rsi, iframe->rdx, iframe->rcx, iframe->r8, iframe->r9);
    }
}

