#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "misc.h"
#include "devices/serial.h"
#include "gdt/gdt.h"
#include "idt/idt.h"
#include "mem/pmm.h"
#include "mem/vmm.h"
#include "paging/paging.h"
#include "bootstub.h"

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

    vmm_init();
    
    hcf();
}
