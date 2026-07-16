#pragma once
#include <stdint.h>

typedef enum: uint8_t {
    INODE_KIND_DIR,
    INODE_KIND_FILE,
} inode_kind_t;

typedef struct inode_t inode_t;

typedef struct 
{
    int (*lookup)();
} inode_ops_t;

typedef struct inode_t
{
    inode_kind_t kind;

} inode_t;
