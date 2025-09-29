#include "mem/pmm.h"
#include "mem/vmm.h"
#include "bootstub.h"
#include "devices/serial.h"
#include "mem/allocators/slab_allocator.h"
#include "mem/pmm.h"

#define HIGHER_HALF_ADDR 0xffff800000000000

extern kernel_params_t *kernel_params;
extern virt_addr_t hhdm_end;

extern char kernel_start[];
extern char kernel_end[];

static virt_mem_region_t *virt_mem_region_list = (virt_mem_region_t*)NULL;
static cache_t *vmm_cache = NULL;

void vmm_init(void)
{
    vmm_cache = (cache_t*)(palloc(sizeof(cache_t)) + kernel_params->hhdm);
    init_cache(vmm_cache, sizeof(cache_t));
}