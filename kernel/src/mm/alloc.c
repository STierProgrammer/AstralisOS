#include <misc/debug.h>
#include <mm/alloc.h>
#include <mm/slab.h>
#include <stddef.h>

#define NUM_KMALLOC_SIZES 14
static size_t kmalloc_sizes[NUM_KMALLOC_SIZES] =
{
    16, 32, 64, 128, 256, 512, 1024, 2048, 4096,
    8192, 16384, 32768, 65536, 131072
};

static cache_t *kmalloc_size_caches[NUM_KMALLOC_SIZES] = { 0 };

void kmalloc_init(void)
{
    for (size_t i = 0; i < NUM_KMALLOC_SIZES; i++)
    {
        kmalloc_size_caches[i] = cache_create("ok", kmalloc_sizes[i]);
    }
}

static cache_t *kmalloc_get_cache(size_t size)
{
    for (size_t i = 0; i < NUM_KMALLOC_SIZES; i++)
    {
        if (size <= kmalloc_sizes[i] && size > 0)
        {
            return kmalloc_size_caches[i];
        }
    }
    return NULL;
}

void* kmalloc(size_t size)
{
    cache_t *cache = kmalloc_get_cache(size);
    if (!cache)
    {
        srdebug(kmalloc, "Failed to allocate for %zu", size);
        return NULL;
    }

    return cache_alloc(cache);
}

void kfree(void *ptr, size_t size)
{
    cache_t *cache = kmalloc_get_cache(size);
    if (!cache)
    {
        srdebug(kfree, "Failed to free %x (with size %zu)", ptr, size);
        return;
    }

    cache_free(cache, ptr);
}


