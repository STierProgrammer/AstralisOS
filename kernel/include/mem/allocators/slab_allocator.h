#pragma once
#include <stddef.h>

#if defined(__x86_64__) || defined(_M_X64)
#include "arch/x86_64/def.h"
#endif

typedef struct cache_t
{
    /// @brief Linked list to the following:
    /// @brief - ```Fully filled blocks``` - Blocks without available memory
    /// @brief - ```Partially filled blocks``` - Blocks with partially available memory
    /// @brief - ```Free blocks``` - Blocks with full memory
    list_t full, partial, free;

    /// @brief Size of a single object in a slab
    size_t obj_size;
    
    /// @brief Amount of objects per slab
    size_t obj_per_slab;
    
    /// @brief Total amount of objects in a cache
    size_t total_objs;
} cache_t;

typedef struct slab_t
{
    /// @brief Linked list of slabs
    list_t slabs;

    /// @brief Cache which this particular slab belongs to
    cache_t* cache;

    /// @brief 
    virt_addr_t addr;
    /// @brief 
    size_t free_idx;
} slab_t;

virt_addr_t cache_alloc(cache_t *cache);
void init_cache(cache_t *cache, size_t obj_size);
cache_t *new_cache(cache_t *main_cache, size_t obj_size);
