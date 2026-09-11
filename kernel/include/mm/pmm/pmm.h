#pragma once
#include <kernel.h>
#include <stddef.h>
#include <stdint.h>
#include <libds/include/bitmap.h>
#include <libds/include/list.h>

extern size_t pmm_free_num_pages;

typedef struct page_t
{
    list_t      list;
    size_t      refcount;
    long        flags;
} page_t;

typedef struct pmm_area_t pmm_area_t;
typedef struct pmm_area_t
{
    paddr_t     addr;
    size_t      num_pages;
    size_t      free_idx_hint;
    pmm_area_t* next;
    bitmap_t    bitmap;
} pmm_area_t;

void        pmm_init(void);
paddr_t     pmm_alloc(size_t size);
paddr_t     pmm_palloc(size_t num_pages);
void        pmm_free(paddr_t addr, size_t size);
void        pmm_pfree(paddr_t addr, size_t num_pages);

