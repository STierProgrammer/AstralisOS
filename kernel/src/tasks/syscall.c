#include "misc/debug.h"
#include "misc/helpers.h"
#include <tasks/syscall.h>
#include <tasks/sched.h>
#include <mm/pmm/pmm.h>

#ifdef __ARCH_X86_64__
#include <arch/x86_64/mm/paging.h>
#endif

enum
{
    SYS_OPEN    = 0,
    SYS_CLOSE   = 1,
    SYS_WRITE   = 2,
    SYS_READ    = 3,
    SYS_SBRK    = 4,
    SYS_MMAP    = 5,
    SYS_MUNMAP  = 6
};

typedef long syscall_fn_t(unsigned long arg1, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5, unsigned long arg6);
#define SYSCALL(NR, func) [NR] = (syscall_fn_t*)(func)
#define NUM_SYSCALLS sizeof(syscall_table)/sizeof(*syscall_table)

typedef long off_t;

int sys_open(const char *path)
{
    srdebug(sys_open, "opened path: %s", path);
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

vaddr_t sys_sbrk(intptr_t increment)
{
    if (increment == 0)
        return sched_curr_task()->heap;

    task_t *task = sched_curr_task();
    vaddr_t ptr = task->heap;
    if (increment > 0 && (task->heap | ~0xFFF))
    {
        pt_map(task->pt, pmm_palloc(1), task->heap, PAGE_FLAG_PRESENT | PAGE_FLAG_READ_WRITE | PAGE_FLAG_USER_SUPERVISOR);
    }
    task->heap += increment;
    return ptr;
}

#define MAP_ANONYMOUS   0

vaddr_t sys_mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset)
{
    
}

int sys_munmap(void *addr, size_t length)
{

}

syscall_fn_t *syscall_table[] =
{
    SYSCALL(SYS_OPEN, sys_open),
    SYSCALL(SYS_CLOSE, sys_close),
    SYSCALL(SYS_WRITE, sys_write),
    SYSCALL(SYS_READ, sys_read),
    SYSCALL(SYS_SBRK, sys_sbrk),
    SYSCALL(SYS_MMAP, sys_mmap),
    SYSCALL(SYS_MUNMAP, sys_munmap),
};

void syscall_handler(interrupt_frame_t *iframe)
{
    uint64_t syscall_number = iframe->rax;   
    
    if (syscall_number < NUM_SYSCALLS)
    {
        iframe->rax = syscall_table[syscall_number](iframe->rdi, iframe->rsi, iframe->rdx, iframe->rcx, iframe->r8, iframe->r9);
    }
}

