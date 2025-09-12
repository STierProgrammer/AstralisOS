#pragma once

#include <stdint.h>
#include "def.h"
#include "bootstub.h"

typedef struct physc_mem_region_t
{
    physc_addr_t base;
    struct physc_mem_region_t *next;
} physc_mem_region_t;

void pmm_init(void);
physc_addr_t palloc(void);
void pfree(physc_addr_t physc_addr);
