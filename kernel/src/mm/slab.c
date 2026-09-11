#include <kernel.h>
#include <mm/vheap.h>
#include <stdbool.h>
#include <libds/include/list.h>
#include <mm/slab.h>

static cache_t *caches_cache = NULL;
static cache_t *slabs_cache = NULL;

static void cache_init(cache_t *cache, const char *name, size_t obj_size)
{
    list_init(&cache->list);
    list_init(&cache->slabs_free);
    list_init(&cache->slabs_partial);
    list_init(&cache->slabs_full);
    
    cache->obj_size = obj_size;
    // TODO: Change this to a better method
    cache->num_pages_per_slab = 1;
    cache->num_objs_per_slab = cache->num_pages_per_slab * PAGE_SIZE / cache->obj_size;
    cache->total_num_objs = 0;
    cache->total_num_slabs = 0;
    cache->name = name;
}

void slab_init(void)
{
    caches_cache = vmalloc(sizeof(cache_t));
    cache_init(caches_cache, "caches cache", sizeof(cache_t));

    slabs_cache = vmalloc(sizeof(cache_t));
    cache_init(slabs_cache, "slabs cache", sizeof(slab_t));
}

// TODO: Maybe tweak with alignment
// TODO: This will corrupt memory if obj_size < 2 * sizeof(uintptr_t)
cache_t *cache_create(const char *name, size_t obj_size)
{
    cache_t *cache = cache_alloc(caches_cache);
    
    // Simple hack to avoid corruption
    if (obj_size < 2 * sizeof(uintptr_t))
        obj_size = 2 * sizeof(uintptr_t);

    cache_init(cache, name, obj_size);
    list_append(&caches_cache->list, &cache->list);

    return cache;
}

bool cache_grow(cache_t *cache)
{
    slab_t *slab = NULL;
    
    if (cache == slabs_cache) {
        slab = vmalloc(sizeof(slab_t));    
    } else {
        slab = cache_alloc(slabs_cache);
    }

    if (!slab)
    {
        return false;
    }

    list_init(&slab->list);
    list_init(&slab->freelist);
    
    slab->num_objs_inuse = 0;
    slab->cache = cache;
    slab->smem = vmalloc(cache->num_objs_per_slab * cache->obj_size);
    
    for (size_t i = 0; i < cache->num_objs_per_slab; ++i)
    {
        list_t *list = slab->smem + i * cache->obj_size;
        list_append(&slab->freelist, list);
    }

    list_append(&cache->slabs_free, &slab->list);

    cache->total_num_slabs++;
    cache->total_num_objs += cache->num_objs_per_slab;

    return true;
}

static inline slab_t *get_slab(list_t *list)
{
    return (slab_t*)list_next(list);
}

static slab_t *slab_select(cache_t *cache)
{
    slab_t *slab = NULL;

    if ((slab = get_slab(&cache->slabs_partial)))
        return slab;

    if ((slab = get_slab(&cache->slabs_free)))
        return slab;  

    if (!cache_grow(cache))
        return slab;
    
    slab = get_slab(&cache->slabs_free);
    return slab;
}

static inline void* get_obj(slab_t *slab)
{
    return (void*)list_next(&slab->freelist);
}

void* cache_alloc(cache_t *cache)
{
    slab_t *slab = slab_select(cache);
    if (!slab)
        return NULL;

    void *obj = get_obj(slab);     
    list_remove(obj);

    slab->num_objs_inuse++;

    list_remove(&slab->list);
    if (slab->num_objs_inuse == cache->num_objs_per_slab)
        list_append(&cache->slabs_full, &slab->list);
    else
        list_append(&cache->slabs_partial, &slab->list);
        
    return obj;
}

static bool cache_free_within(cache_t *cache, slab_t *slab, void *ptr)
{
    foreach (node, slab->list.prev)
    {
        slab_t *curr = (slab_t*)node;
        
        if (curr->smem <= ptr && ptr < curr->smem + cache->num_objs_per_slab * cache->obj_size)
        {
            if (--curr->num_objs_inuse == 0)
            {
                list_remove(&curr->list);
                list_append(&cache->slabs_free, &curr->list);
            } else {
                list_remove(&curr->list);
                list_append(&cache->slabs_partial, &curr->list);
            }
            
            list_append(&curr->freelist, node);

            return true;
        }
    }

    return false;
}

void cache_free(cache_t *cache, void *ptr)
{
    slab_t *slab = get_slab(&cache->slabs_full);
    if (slab)
        if (cache_free_within(cache, slab, ptr))
            return;

    slab = get_slab(&cache->slabs_partial);
    if (slab)
        if (cache_free_within(cache, slab, ptr))
            return;
}

bool cache_shrink(cache_t *cache)
{    
    slab_t *slab = get_slab(&cache->slabs_free);
    if (!slab) 
        return false;

    list_remove(&slab->list);
    
    vfree(slab->smem);

    if (cache == slabs_cache)
        vfree(slab);
    else
        cache_free(slabs_cache, slab);

    cache->total_num_slabs--;
    cache->total_num_objs -= cache->num_objs_per_slab;

    return true;
}

list_t *cache_chain(void)
{
    return &caches_cache->list;
}

