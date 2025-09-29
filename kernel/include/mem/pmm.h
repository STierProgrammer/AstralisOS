#pragma once

#if defined(__x86_64__) || defined(_M_X64)
#include "arch/x86_64/def.h"
#endif
#include <stddef.h>
#include <stdint.h>

#include "bootstub.h"

typedef struct physc_mem_region_t
{
    size_t pages;
    struct physc_mem_region_t *next;
} physc_mem_region_t;

void pmm_print(void);
void pmm_init(void);
physc_addr_t palloc(size_t size);
void pfree(physc_addr_t physc_addr, size_t pages);
