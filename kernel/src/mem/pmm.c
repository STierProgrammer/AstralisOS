#include <stddef.h>
#include "mem/pmm.h"
#include "devices/serial.h"

extern kernel_params_t *kernel_params;
static physc_mem_region_t *free_mem_head = (physc_mem_region_t*)NULL;

void pmm_init(void)
{
    memmap_t* memmap = &kernel_params->memmap;

    for (size_t i = 0; i < memmap->entry_count; i++)
    {
        if (memmap->entries[i].type == MEMMAP_USABLE)
        {
            size_t length = memmap->entries[i].length;
            
            physc_addr_t base = ALIGN_UP(memmap->entries[i].base, PAGE_SIZE);
            physc_addr_t end = ALIGN_DOWN((memmap->entries[i].base + length), PAGE_SIZE);

            for (physc_addr_t current = base; current < end; current += PAGE_SIZE)
            {
                physc_mem_region_t *region = (physc_mem_region_t*)(current + kernel_params->hhdm);
                region->base = current;
                region->next = free_mem_head;
                free_mem_head = region;
            }
        }
    }

    srprintf("[PMM Initialized]\n");
}

physc_addr_t palloc(void)
{
    if (!free_mem_head) return (physc_addr_t)NULL;
    
    physc_mem_region_t *region = free_mem_head;
    free_mem_head = free_mem_head->next;
    
    return region->base;
}

void pfree(physc_addr_t physc_addr)
{
    physc_mem_region_t *region = (physc_mem_region_t*)(physc_addr + kernel_params->hhdm);
    region->next = free_mem_head;
    region->base = physc_addr;
    free_mem_head = region;
}
