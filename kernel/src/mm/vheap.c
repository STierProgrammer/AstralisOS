#ifdef __ARCH_X86_64__
#include <arch/x86_64/mm/paging.h>
#endif

#include <misc/helpers.h>
#include <kernel.h>
#include <mm/vheap.h>
#include <mm/vmm/vmm.h>

static vmm_t vheap;
extern vaddr_t hhdm_end;

void vheap_init(void)
{
    vmm_init(&vheap, krnlctx(pt), hhdm_end, (32 * 1024 * 1024 * 1024l) / PAGE_SIZE);
}

void *vmalloc_pf(size_t size, page_flags_t flags)
{
    size_t pages = align_up(size, PAGE_SIZE) / PAGE_SIZE;
    return (void*)vmm_palloc(&vheap, pages, flags);
}

void *vmalloc(size_t size)
{
    return vmalloc_pf(size, PAGE_FLAG_READ_WRITE);
}

void vfree(void *ptr)
{
    vmm_free(&vheap, (vaddr_t)ptr);
}

