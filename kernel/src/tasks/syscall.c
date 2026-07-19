#include "misc/debug.h"
#include <tasks/syscall.h>
#include <tasks/sched.h>

enum
{
    SYS_OPEN    = 0,
    SYS_CLOSE   = 1,
    SYS_WRITE   = 2,
    SYS_READ    = 3,
};

typedef long syscall_fn_t(unsigned long arg1, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5, unsigned long arg6);
#define SYSCALL(NR, func) [NR] = (syscall_fn_t*)(func)
#define NUM_SYSCALLS sizeof(syscall_table)/sizeof(*syscall_table)

int sys_open(const char *path)
{
    srdebug(sys_open, "opened path");
    return task_open(sched_curr_task(), path);
}
 
int sys_close(int fd)
{
    srdebug(sys_close, "closing fd: %d", fd);
    return task_close(sched_curr_task(), fd);
}
 
long sys_write(int fd, const void *buf, size_t count)
{
    srdebug(sys_write, "writing to fd: %d", fd);
    return task_write(sched_curr_task(), fd, buf, count);
}

long sys_read(int fd, void *buf, size_t count)
{
    srdebug(sys_read, "reading from fd: %d", fd);
    return task_read(sched_curr_task(), fd, buf, count);
}

syscall_fn_t *syscall_table[] =
{
    SYSCALL(SYS_OPEN, sys_open),
    SYSCALL(SYS_CLOSE, sys_close),
    SYSCALL(SYS_WRITE, sys_write),
    SYSCALL(SYS_READ, sys_read),
};

void syscall_handler(interrupt_frame_t *iframe)
{
    uint64_t syscall_number = iframe->rax;   
    
    if (syscall_number < NUM_SYSCALLS)
    {
        iframe->rax = syscall_table[syscall_number](iframe->rdi, iframe->rsi, iframe->rdx, iframe->rcx, iframe->r8, iframe->r9);
    }
}

