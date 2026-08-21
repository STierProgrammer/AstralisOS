#pragma once
#include <kernel.h>

typedef struct tty_t tty_t;
typedef struct {
    ssize_t (*write)(tty_t*, const char*, size_t);
    long    (*ioctl)(tty_t*, unsigned long, unsigned long);        
} tty_ops_t;

typedef struct tty_t {
    const tty_ops_t *ops;
} tty_t;
