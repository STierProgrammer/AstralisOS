#ifdef REWRITE
#pragma once

#include <mm/slab.h>
#include <misc/strview.h>
#include <stddef.h>
#include <sys/types.h>

/* Owner */
#define S_IRUSR 00400 // read
#define S_IWUSR 00200 // write
#define S_IXUSR 00100 // exec

#define S_IRGRP 00040 // read
#define S_IWGRP 00020 // write
#define S_IXGRP 00010 // exec

#define S_IROTH 00004 // read
#define S_IWOTH 00002 // write
#define S_IXOTH 00001 // exec

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
    int (*create)(inode_t *parent, const char *name, size_t namelen, mode_t mode, inode_t **ret);
    int (*mkdir)(inode_t *parent, const char *name, size_t namelen, mode_t mode, inode_t **ret);
    int (*lookup)(inode_t *parent, const char *name, size_t namelen, inode_t **ret);
    

} inode_ops_t;

typedef struct inode_t 
{
    ino_t ino;
    inode_kind_t kind;
    const inode_ops_t *ops;
    void* priv;
    
    size_t  refcount;
    size_t  size;
    mode_t  mode;
    uid_t   uid;
    gid_t   gid;
} inode_t;

typedef struct qstr_t
{
    uint32_t hash;
    size_t namelen;
    const char *name;
} qstr_t;

typedef struct dentry_t dentry_t;
typedef struct dentry_t
{
    size_t refcount;
    inode_t *inode;
    dentry_t *parent;
    qstr_t name;
} dentry_t;

typedef struct fd_t {
    inode_t *inode;
    long flags;
    off_t offset;
} fd_t;

typedef struct fd_ops_t {

} fd_ops_t;

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
#endif

