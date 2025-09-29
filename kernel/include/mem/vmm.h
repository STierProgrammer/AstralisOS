#pragma once

#if defined(__x86_64__) || defined(_M_X64)
#include "arch/x86_64/def.h"
#endif

#include <stddef.h>

typedef struct virt_mem_region_t
{
    virt_addr_t base;
    size_t size;
    struct virt_mem_region_t *next, *prev;
} virt_mem_region_t;

void vmm_print(void);
void vmm_init(void);
virt_mem_region_t* vmm_alloc(size_t size);
void vmm_free(virt_mem_region_t* addr);
