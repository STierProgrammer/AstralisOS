#include "mem/pmm.h"
#include "devices/serial.h"

extern kernel_params_t *kernel_params;
static physc_mem_region_t *free_mem_region_list = (physc_mem_region_t*)NULL;

void pmm_print(void)
{
    physc_mem_region_t *curr = free_mem_region_list;
    while (curr)
    {
        srprintf("[PMM]: Base: %x, Pages: %x, Next Base: %x\n", curr, curr->pages, curr->next ? curr->next : 0);
        curr = curr->next;
    }
    srprintf("\n");
}

void pmm_init(void)
{
    memmap_t *memmap = &kernel_params->memmap;

    for (size_t i = 0; i < memmap->entry_count; i++)
    {
        if (memmap->entries[i].type == MEMMAP_USABLE)
        {
            size_t length = memmap->entries[i].length;
            physc_addr_t base = ALIGN_UP(memmap->entries[i].base, PAGE_SIZE);

            physc_mem_region_t *region = (physc_mem_region_t*)(base + kernel_params->hhdm);
            region->next = free_mem_region_list;
            region->pages = length / PAGE_SIZE;
            free_mem_region_list = region;
        }
    }

    srprintf("[PMM Initialized]\n");
}

physc_addr_t palloc(size_t size)
{
    size_t pages = ALIGN_UP(size, PAGE_SIZE) / PAGE_SIZE;
    physc_mem_region_t *curr = free_mem_region_list;
    physc_mem_region_t *prev = (physc_mem_region_t*)NULL;

    while (curr)
    {
        if (curr->pages > pages)
        {
            curr->pages -= pages;
            return (physc_addr_t)((physc_addr_t)curr + pages * PAGE_SIZE - kernel_params->hhdm);
        } 
        else if (curr->pages == pages)
        {
            if (!prev) free_mem_region_list = curr->next;
            else prev->next = curr->next;
            return (physc_addr_t)curr - kernel_params->hhdm;
        }

        prev = curr;
        curr = curr->next;
    }

    return (physc_addr_t)NULL;
}

void pfree(physc_addr_t physc_addr, size_t pages)
{
    physc_mem_region_t *region = (physc_mem_region_t*)(physc_addr + kernel_params->hhdm);
    region->next = free_mem_region_list;
    region->pages = pages;
    free_mem_region_list = region;
}
