#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#if defined(__x86_64__) || defined(_M_X64)
#include "arch/x86_64/def.h"
#include "arch/x86_64/mem/paging.h"
#include "arch/x86_64/gdt/gdt.h"
#include "arch/x86_64/idt/idt.h"
#endif

#include "devices/serial.h"
#include "mem/pmm.h"
#include "mem/vmm.h"
#include "bootstub.h"
#include "devices/pic.h"
#include "mem/allocators/slab_allocator.h"

page_table_t *pml4;
kernel_params_t *kernel_params;

void kmain(kernel_params_t *params)
{
    kernel_params = params;

    init_serial();
    init_gdt();
    init_idt();
    pmm_init();
    init_pml4();
    map_kernel();
    map_memmap();
    set_cr3((uint64_t)(((uint64_t)pml4) - kernel_params->hhdm));

    typedef struct wtvr {
        uint16_t x;
        uint16_t y;
    } wtvr;

    cache_t *cache = (cache_t*)(palloc(sizeof(cache_t)) + kernel_params->hhdm);
    init_cache(cache, sizeof(cache));

    cache_t *wtvr_cache = (cache_t*)cache_alloc(cache);
    wtvr *p = NULL;
    init_cache(wtvr_cache, sizeof(wtvr));
    p = (wtvr*)cache_alloc(wtvr_cache);
    srprintf("%x\n", p);
    p->x = 4;
    p->y = 5;
    srprintf("%x\n", p->x);
    srprintf("%x\n", p->y);

    hcf();
}
