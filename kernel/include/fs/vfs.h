#pragma once

#include <mm/slab.h>
#include <misc/strview.h>
#include <stddef.h>

typedef struct path_t path_t;
typedef struct inode_t inode_t;

typedef enum inode_kind_t 
{
    INODE_DIR,
    INODE_FILE,

    INODE_KIND_COUNT
} inode_kind_t;

typedef struct inode_ops_t 
{
    int  (*create)(inode_t *dir, const char *name, size_t name_len, inode_kind_t kind, inode_t **ret);
    int  (*lookup)(inode_t *dir, const char *name, size_t name_len, inode_t **ret);
    long (*read)  (inode_t *inode, void *buf, size_t count, size_t offset);
    long (*write) (inode_t *inode, const void *buf, size_t count, size_t offset);
    int  (*ioctl) (inode_t *inode, size_t id, void *arg);
} inode_ops_t;

typedef struct inode_t 
{
    inode_kind_t        kind;
    const inode_ops_t * ops;
    void *              priv;
} inode_t;

typedef struct superblock_t 
{

} superblock_t;

typedef struct fs_t {
    int (*mount)(inode_t **root);
} fs_t;

typedef struct path_t {
    inode_t *root, *from;
    strview_t strview;
} path_t;

extern inode_t *vfs_root;
extern cache_t* inode_cache;

long        inode_read  (inode_t *inode, void *buf, size_t count, size_t offset);
long        inode_write (inode_t *inode, const void *buf, size_t count, size_t offset);
int         inode_lookup(inode_t *dir, const char *name, size_t name_len, inode_t **ret);
int         inode_create(inode_t *dir, const char *name, size_t name_len, inode_kind_t kind, inode_t **ret);
int         fs_mount    (const fs_t *fs, inode_t **root);

path_t      vfs_path_from_abs(const char *cstr);

int         vfs_create(const path_t *path, inode_kind_t kind, inode_t **ret);
int         vfs_lookup(const path_t *path, inode_t **ret);
int         vfs_mount(const fs_t *fs, const path_t *path);

void        vfs_init(void);
