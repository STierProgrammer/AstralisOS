#pragma once

#include "def.h"
#include <stddef.h>

typedef struct virt_mem_region_t
{
    virt_addr_t base;
    size_t length;
    struct virt_mem_region_t *next;
} virt_mem_region_t;

void vmm_init(void);
virt_mem_region_t* vmm_alloc(size_t size);
