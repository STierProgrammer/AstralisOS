#pragma once

#include <kernel.h>
#include <stdint.h>
#include <libds/include/list.h>
#include <fs/vfs.h>
#include <tasks/elf.h>

typedef struct 
{
    void*  sp;
    void*  base;
    size_t size;
} stack_t;

typedef struct {
    inode_t *inode;
    size_t   offset;
} file_t;

typedef struct
{ 
    list_t          list;
    stack_t         kernel_stack;
    stack_t         user_stack;
    vaddr_t         heap;
    size_t          pid;
    page_table_t*   pt;
    file_t          fd[256];
} task_t;

void sched_init(void);
void sched_switch(void);
void sched_schedule(task_t *task);
void sched_unschedule(task_t *task);
task_t *sched_curr_task(void);

task_t* kernel_task_create(void (*entry)());
task_t *user_task_create(Elf64_Ehdr *hdr);

void    task_sleep(void);
void    task_wake_all_up(void);
void    task_yield(void);

int     task_open(task_t *task, const char *path);
int     task_close(task_t *task, int fd);
long    task_read(task_t *task, int fd, void *buf, size_t count);
long    task_write(task_t *task, int fd, const void *buf, size_t count);

