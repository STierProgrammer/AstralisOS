#pragma once

#include <kernel.h>
#include <stdint.h>
#include <libds/include/list.h>

typedef struct slab_freelist_t slab_freelist_t;
typedef struct slab_freelist_t 
{
    slab_freelist_t *next;
} slab_freelist_t;

/* TODO: Add colouring */
/* TODO: Add flags */
typedef struct
{
    list_t list;
    list_t slabs_free;
    list_t slabs_partial;
    list_t slabs_full;
    
    size_t obj_size;
    size_t num_objs_per_slab;

    size_t total_num_objs;
    size_t total_num_slabs;
    
    /* It's usually 1 */
    size_t num_pages_per_slab;
    const char *name;
} cache_t;

typedef struct
{
    list_t list;
    cache_t *cache;

    size_t num_objs_inuse;
    void *smem;
    // TODO: prev pointer is not needed
    list_t freelist;
} slab_t;

void     slab_init(void);
cache_t* cache_create(const char *name, size_t obj_size);
bool     cache_grow(cache_t *cache);
bool     cache_shrink(cache_t *cache);
void*    cache_alloc(cache_t *cache);
void     cache_free(cache_t *cache, void *ptr);
list_t * cache_chain(void);

