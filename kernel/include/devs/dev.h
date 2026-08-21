#pragma once

#include <stdint.h>
#include <stddef.h>
#include <libds/include/list.h>
#include <kernel.h>

typedef ssize_t(*write_fn_t)(const void *buf, size_t count);
typedef ssize_t(*read_fn_t)(void *buf, size_t count);

enum  
{
    DEV_MAJOR_INPUT
};

enum
{
    DEV_MINOR_KBD,
    DEV_MINOR_MSE,
};

typedef struct 
{
    list_t      list;
    const char* name;
    write_fn_t  write;
    read_fn_t   read;

    int major;
    int minor;
} dev_t;

void        devs_init(void);
dev_t*      dev_create(const char *name, int major, int minor, write_fn_t write, read_fn_t read);
ssize_t     dev_write(dev_t *device, const void *buf, size_t count);
ssize_t     dev_read(dev_t *device, void *buf, size_t count);
dev_t*      dev_find(const char *name);

