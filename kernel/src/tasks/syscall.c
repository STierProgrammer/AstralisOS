#include "misc/debug.h"
#include "misc/helpers.h"
#include <tasks/syscall.h>
#include <tasks/sched.h>
#include <mm/pmm/pmm.h>

#ifdef __ARCH_X86_64__
#include <arch/x86_64/mm/paging.h>
#endif

#define SYSCALL_SIG(name) long sys_##name(long arg1, long arg2, long arg3, long arg4, long arg5, long arg6)

#define SYSCALL_DEFINE1(ret, name, tp1, a1)     \
    static ret _sys_##name(tp1 a1);             \
    SYSCALL_SIG(name) {                         \
        return (long)_sys_##name((tp1)arg1);    \
    }                                           \
    static ret _sys_##name(tp1 a1)

#define SYSCALL_DEFINE2(ret, name, tp1, a1, tp2, a2)        \
    static ret _sys_##name(tp1 a1, tp2 a2);                 \
    SYSCALL_SIG(name) {                                     \
        return (long)_sys_##name((tp1)arg1, (tp2)arg2);     \
    }                                                       \
    static ret _sys_##name(tp1 a1, tp2 a2)

#define SYSCALL_DEFINE3(ret, name, tp1, a1, tp2, a2, tp3, a3)           \
    static ret _sys_##name(tp1 a1, tp2 a2, tp3 a3);                     \
    SYSCALL_SIG(name) {                                                 \
        return (long)_sys_##name((tp1)arg1, (tp2)arg2, (tp3)arg3);      \
    }                                                                   \
    static ret _sys_##name(tp1 a1, tp2 a2, tp3 a3)

#define SYSCALL_DEFINE4(ret, name, tp1, a1, tp2, a2, tp3, a3, tp4, a4)              \
    static ret _sys_##name(tp1 a1, tp2 a2, tp3 a3, tp4 a4);                         \
    SYSCALL_SIG(name) {                                                             \
        return (long)_sys_##name((tp1)arg1, (tp2)arg2, (tp3)arg3, (tp4)arg4);       \
    }                                                                               \
    static ret _sys_##name(tp1 a1, tp2 a2, tp3 a3, tp4 a4)

#define SYSCALL_DEFINE5(ret, name, tp1, a1, tp2, a2, tp3, a3, tp4, a4, tp5, a5)                 \
    static ret _sys_##name(tp1 a1, tp2 a2, tp3 a3, tp4 a4, tp5 a5);                             \
    SYSCALL_SIG(name) {                                                                         \
        return (long)_sys_##name((tp1)arg1, (tp2)arg2, (tp3)arg3, (tp4)arg4, (tp5)arg5);        \
    }                                                                                           \
    static ret _sys_##name(tp1 a1, tp2 a2, tp3 a3, tp4 a4, tp5 a5)

#define SYSCALL_DEFINE6(ret, name, tp1, a1, tp2, a2, tp3, a3, tp4, a4, tp5, a5, tp6, a6)         \
    static ret _sys_##name(tp1 a1, tp2 a2, tp3 a3, tp4 a4, tp5 a5, tp6 a6);                     \
    SYSCALL_SIG(name) {                                                                                     \
        return (long)_sys_##name((tp1)arg1, (tp2)arg2, (tp3)arg3, (tp4)arg4, (tp5)arg5, (tp6)arg6);         \
    }                                                                                                       \
    static ret _sys_##name(tp1 a1, tp2 a2, tp3 a3, tp4 a4, tp5 a5, tp6 a6)

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

typedef long syscall_fn_t(long arg1, long arg2, long arg3, long arg4, long arg5, long arg6);
#define SYSCALL(NR, func) [NR] = func
#define NUM_SYSCALLS sizeof(syscall_table)/sizeof(*syscall_table)

typedef long off_t;

SYSCALL_DEFINE1(int, open, const char *, path)
{
    srdebug(sys_open, "opened path: %s", path);
    return task_open(sched_curr_task(), path);
}

SYSCALL_DEFINE1(int, close, int, fd)
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
    (void)addr;
    (void)length;
    (void)prot;
    (void)flags;
    (void)fd;
    (void)offset;
}

int sys_munmap(void *addr, size_t length)
{
    (void)addr;
    (void)length;
}

syscall_fn_t *syscall_table[] =
{
    SYSCALL(SYS_OPEN, sys_open),
    SYSCALL(SYS_CLOSE, sys_close),
};

void syscall_handler(interrupt_frame_t *iframe)
{
    uint64_t syscall_number = iframe->rax;   
    
    if (syscall_number < NUM_SYSCALLS)
    {
        iframe->rax = syscall_table[syscall_number](iframe->rdi, iframe->rsi, iframe->rdx, iframe->rcx, iframe->r8, iframe->r9);
    }
}

