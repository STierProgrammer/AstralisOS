#include "mm/slab.h"
#include "mm/vheap.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <fs/vfs.h>
#include <mm/vmm/vmm.h>
#include <misc/err.h>
#include <misc/strview.h>

#define TMPFS_MAX_NAME_LEN          128
#define TMPFS_DATA_CHUNK_SIZE       4096
#define MAX_ENTRIES_PER_DATA_CHUNK  ((TMPFS_DATA_CHUNK_SIZE - sizeof(tmpfs_data_t)/sizeof(inode_t*)))
#define MAX_FILE_DATA_PER_CHUNK     ((TMPFS_DATA_CHUNK_SIZE - sizeof(tmpfs_data_t)))

#define file_data(data)((char*)((data)+1))
#define dir_entires(data) ((inode_t**)((data) + 1))

const inode_ops_t dir_ops;
const inode_ops_t file_ops;

typedef struct tmpfs_data_t tmpfs_data_t;
typedef struct tmpfs_data_t {
    tmpfs_data_t *next;
} tmpfs_data_t;

typedef struct tmpfs_inode_t {
    char name[TMPFS_MAX_NAME_LEN];
    size_t cap;
    tmpfs_data_t *data;
} tmpfs_inode_t;


inode_t *tmpfs_inode_create(inode_kind_t kind, const inode_ops_t *ops)
{
    inode_t *inode = cache_alloc(inode_cache);
    if (inode)
    {
        tmpfs_inode_t *tmpfs_inode = vmalloc(sizeof(inode_t));
        if (tmpfs_inode)
        {
            inode->kind = kind;
            inode->ops = ops;
            inode->priv = tmpfs_inode;
            tmpfs_inode->cap = 0;
            tmpfs_inode->data = NULL;
        } else {
            cache_free(inode_cache, inode);
            return NULL;
        }
    }

    return inode;
}

tmpfs_data_t *tmpfs_new_data_chunk()
{
    tmpfs_data_t *data = vmalloc(TMPFS_DATA_CHUNK_SIZE);
    if (data) memset(data, 0,TMPFS_DATA_CHUNK_SIZE);
    return data;
}

int tmpfs_lookup(inode_t *dir, const char *name, size_t name_len, inode_t **ret)
{
    tmpfs_inode_t *tmpfs_inode = dir->priv;
    tmpfs_data_t *data =  tmpfs_inode->data;
    size_t size = tmpfs_inode->cap;

    while (data && size > 0)
    {
        for (size_t i = 0; i < size && i < MAX_ENTRIES_PER_DATA_CHUNK; i++)
        {
            inode_t *child_node = dir_entires(data)[i];
            if (!child_node) continue;
            tmpfs_inode_t *child_tmpfs_inode = child_node->priv;
            if (strlen(child_tmpfs_inode->name) == name_len 
                &&  memcmp(child_tmpfs_inode->name, name, name_len) == 0
            ) {
                *ret = child_node;
                return SUCCESS;
            }
        }

        size -= size < MAX_ENTRIES_PER_DATA_CHUNK ? size : MAX_ENTRIES_PER_DATA_CHUNK;
        data = data->next;
    }

    return -FILE_NOT_FOUND;
}

int tmpfs_create(inode_t *dir, const char *name, size_t name_len, inode_kind_t kind, inode_t **ret)
{
    tmpfs_inode_t *parent_data = dir->priv;
    size_t idx = parent_data->cap;

    tmpfs_data_t **prev_next = &parent_data->data,
                 *chunk = parent_data->data;
    while (idx >= MAX_ENTRIES_PER_DATA_CHUNK)
    {
        if (!chunk) {
            chunk = *prev_next = tmpfs_new_data_chunk();
            if (!chunk) return -OUT_OF_MEMORY;
        }

        idx -= MAX_ENTRIES_PER_DATA_CHUNK;
        prev_next = &chunk->next;
        chunk = chunk->next;
    }

    if (!chunk) {
        *prev_next = chunk = tmpfs_new_data_chunk();
        if (!chunk) return -OUT_OF_MEMORY;
    }

    inode_t *inode = kind == INODE_DIR ? tmpfs_inode_create(INODE_DIR, &dir_ops)
        : tmpfs_inode_create(INODE_FILE, &file_ops);
    if (!inode) return -OUT_OF_MEMORY;

    tmpfs_inode_t *tmpfs_inode = inode->priv;
    if (name_len >= TMPFS_MAX_NAME_LEN)
    {
        // FIXME: Free
        return -NAME_LIMIT;
    }

    memcpy(tmpfs_inode->name, name, name_len);

    dir_entires(chunk)[idx] = inode;
    parent_data->cap++;
    *ret = inode;
    return SUCCESS;
}

int tmpfs_rmfile(inode_t *file)
{
    (void)file;
    return -UNIMPLEMENTED;
}

long tmpfs_read(inode_t *inode, void *buf, size_t count, size_t offset)
{
    if (!inode || !inode->priv || (!buf && count > 0)) return -FILE_NOT_FOUND;
    if (count == 0) return 0;

    tmpfs_inode_t *tmpfs_inode = inode->priv;
    tmpfs_data_t  *curr        = tmpfs_inode->data;

    if (!curr) return -FILE_NOT_FOUND;
    if (offset >= tmpfs_inode->cap) return -OUT_OF_BOUNDS;
    if (count > tmpfs_inode->cap - offset) count = tmpfs_inode->cap - offset;

    while (offset >= MAX_FILE_DATA_PER_CHUNK)
    {
        if (!curr) return -OUT_OF_BOUNDS;
        offset -= MAX_FILE_DATA_PER_CHUNK;
        curr = curr->next;
    }

    size_t read_total = 0;

    while (read_total < count)
    {
        if (!curr) break;
    
        size_t chunk_space = MAX_FILE_DATA_PER_CHUNK - offset;
        size_t read_now = (count - read_total < chunk_space) ? (count - read_total) : chunk_space;

        memcpy(buf + read_total, file_data(curr) + offset, read_now);
        
        read_total += read_now;
        offset = 0;
        curr = curr->next;
    }

    return (long)read_total;
}

long tmpfs_write(inode_t *inode, const void *in_buf, size_t count, size_t offset)
{
    if (!inode || !inode->priv || (!in_buf && count > 0)) return -FILE_NOT_FOUND;
    if (count == 0) return 0;

    tmpfs_inode_t *tmpfs_inode = inode->priv;
    size_t start_offset = offset;

    tmpfs_data_t **prev_next = &tmpfs_inode->data;
    tmpfs_data_t *curr = tmpfs_inode->data;

    if (offset > tmpfs_inode->cap) return -OUT_OF_BOUNDS;

    while (offset >= MAX_FILE_DATA_PER_CHUNK)
    {
        if (!curr) {
            curr = *prev_next = tmpfs_new_data_chunk();
            if (!curr) return -OUT_OF_MEMORY;
        }

        offset -= MAX_FILE_DATA_PER_CHUNK;
        prev_next = &curr->next;
        curr = curr->next;
    }

    size_t written_total = 0;

    while (written_total < count)
    {
        if (!curr) {
            curr = *prev_next = tmpfs_new_data_chunk();
            if (!curr) return -OUT_OF_MEMORY;
        }
        
        size_t chunk_space = MAX_FILE_DATA_PER_CHUNK - offset;
        size_t write_now   = (count - written_total < chunk_space) ? (count - written_total) : chunk_space;

        memcpy(file_data(curr) + offset, (uint8_t*)in_buf + written_total, write_now);

        written_total += write_now;
        offset = 0;
        prev_next = &curr->next;
        curr = curr->next;
    }

    if (tmpfs_inode->cap < start_offset + written_total)
        tmpfs_inode->cap = start_offset + written_total;

    return (long)written_total;
}

int tmpfs_mount_root(inode_t **root)
{
    

    return (*root = tmpfs_inode_create(INODE_DIR, &dir_ops)) ? SUCCESS : -OUT_OF_MEMORY;
}

const inode_ops_t file_ops = {
    .read = tmpfs_read,
    .write = tmpfs_write,
};

const inode_ops_t dir_ops = {
    .create = tmpfs_create,
    .lookup = tmpfs_lookup,
};

const fs_t tmpfs = {
    .mount = tmpfs_mount_root
};

