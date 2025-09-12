#include "vmm.h"
#include "paging/paging.h"
#include "mem/pmm.h"
#include "bootstub.h"
#include "devices/serial.h"

#define HIGHER_HALF_ADDR 0xffff800000000000

extern kernel_params_t *kernel_params;
extern virt_addr_t hhdm_end;

extern char kernel_start[];
extern char kernel_end[];

static virt_mem_region_t *virt_mem_region_list = (virt_mem_region_t*)NULL;

void vmm_init(void)
{
    virt_mem_region_t *hhdm_region = (virt_mem_region_t*)(palloc() + kernel_params->hhdm);
    hhdm_region->base = kernel_params->hhdm - HIGHER_HALF_ADDR;
    hhdm_region->length = hhdm_end - kernel_params->hhdm;

    virt_mem_region_t *kernel_region = (virt_mem_region_t*)(palloc() + kernel_params->hhdm);
    kernel_region->base = (virt_addr_t)kernel_start - HIGHER_HALF_ADDR;
    kernel_region->length = ALIGN_UP((virt_addr_t)kernel_end - (virt_addr_t)kernel_start, PAGE_SIZE);

    hhdm_region->next = kernel_region;
    kernel_region->next = NULL;

    virt_mem_region_list = hhdm_region;

    srprintf("[VMM Initialized]\n");
}

virt_mem_region_t* vmm_alloc(size_t size)
{
    size = ALIGN_UP(size, PAGE_SIZE);
    virt_mem_region_t *curr = virt_mem_region_list;
    virt_mem_region_t *prev = (virt_mem_region_t*)NULL;
    
    virt_addr_t base = 0;
    
    while (curr)
    {
        base = (prev == NULL) ? 0 : prev->base + prev->length;
        if (base + size <= curr->base) break;
        prev = curr;
        curr = curr->next; 
    }

    virt_mem_region_t* region = (virt_mem_region_t*)(palloc() + kernel_params->hhdm);
    region->base = base;
    region->length = size;
    region->next = curr;

    if (prev) {
        prev->next = region;
    } else {
        virt_mem_region_list = region;
    }

    return region;
}
