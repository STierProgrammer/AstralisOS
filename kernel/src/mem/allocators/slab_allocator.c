#include "mem/pmm.h"
#include "bootstub.h"
#include "libc/string.h"
#include "mem/allocators/slab_allocator.h"
#include "devices/serial.h"

extern kernel_params_t *kernel_params;

#define slab_bufctl(slab) ((uint32_t*)(slab + 1))

/// @brief Calculates the amount of memory a slab takes
/// @param cache The cache in which you want to know the amount of memory slab takes 
/// @return Amount of memory slab requires
static inline size_t slab_mem(cache_t *cache)
{
    return cache->obj_size * cache->obj_per_slab + sizeof(slab_t) + sizeof(uint32_t);
}

void cache_grow(cache_t *cache)
{
    virt_addr_t addr = (palloc(slab_mem(cache)) + kernel_params->hhdm);
    if (!addr) return;
    
    slab_t *slab = (slab_t*)addr;
    
    slab->cache = cache;
    slab->addr = addr + sizeof(slab_t) + cache->obj_per_slab;
    slab->free_idx = 0;
    
    cache->total_objs += cache->obj_per_slab;

    for (size_t i = 0; i < cache->obj_per_slab - 1; i++)
    {
        slab_bufctl(slab)[i] = i;
    }

    list_append((list_t*)slab, &cache->free);
}

static slab_t *cache_select(cache_t *cache)
{
    slab_t *slab = (slab_t*)NULL;

    if (slab = (slab_t*)list_next(&cache->partial)) return slab;
    if (slab = (slab_t*)list_next(&cache->free)) return slab;
    
    cache_grow(cache);
    slab = (slab_t*)list_next(&cache->free);
    
    return slab;
}

virt_addr_t cache_alloc(cache_t *cache)
{
    slab_t *slab = cache_select(cache);
    if (!slab) return (virt_addr_t)NULL;

    size_t index = slab_bufctl(slab)[slab->free_idx];
    virt_addr_t ptr = slab->addr + cache->obj_size * index;

    if (slab->free_idx++ == 0)
    {
        list_remove(&slab->slabs);
        list_append(&cache->partial, &slab->slabs);
    }

    if (slab->free_idx == cache->obj_per_slab)
    {
        list_remove(&slab->slabs);
        list_append(&cache->full, &slab->slabs);
    }

    return ptr;
}

cache_t *new_cache(cache_t *main_cache, size_t obj_size)
{
    cache_t *cache = (cache_t*)cache_alloc(main_cache);
    if (!cache) return NULL;
    init_cache(cache, obj_size);
    return cache;
}

void init_cache(cache_t *cache, size_t obj_size)
{
    memset(cache, 0, sizeof(cache_t));
    list_init(&cache->partial);
    list_init(&cache->full);
    list_init(&cache->free);

    cache->obj_size = obj_size;
    cache->obj_per_slab = ALIGN_UP(obj_size, PAGE_SIZE);
}
