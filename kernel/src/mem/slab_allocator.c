#include "mem/pmm.h"
#include "def.h"

// NOTE: This implementation is a slightly modified version of that of the linux kernel itself.
// https://github.com/torvalds/linux/blob/b1bc554e009e3aeed7e4cfd2e717c7a34a98c683/tools/firewire/list.h
//
// Things like list_insert, list_append are all literally the exact same, with the exception 
// of list_remove which also closes in the array element removed since the linux version didn't clean it up
typedef struct list_t 
{
    struct list_t *next, *prev;
} list_t;

static inline void list_init(list_t *list) {
    list->next = list;
    list->prev = list;
}

static inline list_t *list_last(list_t *list) {
    return list->prev != list ? list->prev : NULL;
}

static inline list_t *list_next(list_t *list) {
    return list->next != list ? list->next : NULL;
}

static inline void list_insert(list_t *new, list_t *link) {
    new->prev = link->prev;
    new->next = link;
    new->prev->next = new;
    new->next->prev = new;
}

static inline void list_append(list_t *new, list_t *into) {
    list_insert(new, into);
}

static inline void list_remove(list_t *list) {
    list->prev->next = list->next;
    list->next->prev = list->prev;
    list->next = list;
    list->prev = list;
}

typedef struct cache_t
{
    list_t slabs_full, slabs_partial, slabs_free;
    size_t obj_size;
    size_t obj_per_slab;
} cache_t;

typedef struct slab_t
{
    list_t slabs;
    virt_addr_t addr;
    cache_t* cache;
} slab_t;

static inline size_t slab_mem(cache_t *cache)
{
    return cache->obj_size * cache->obj_per_slab + sizeof(slab_t);
}

void cache_grow(cache_t *cache)
{

}
