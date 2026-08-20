#include <kernel.h>

#include <string.h>

#include <mm/pmm/pmm.h>
#include <misc/helpers.h>
#include <misc/debug.h>



static pmm_area_t *area_list  = NULL;
size_t pmm_free_num_pages = 0;

void pmm_init(void)
{
    memmap_t memmap = bootctx(memmap);
    for (size_t i = 0; i < memmap.num_entries; ++i)
    {
        memmap_entry_t entry;
        memmap.get_entry(&memmap, &entry, i);

        if (entry.type == MEMMAP_USABLE)
        {
            paddr_t base = align_up(entry.base, PAGE_SIZE);
            size_t  len  = align_down(entry.length, PAGE_SIZE);
            size_t  num_pages = len / PAGE_SIZE;
    
            if (num_pages > 1)
            {
                size_t num_free_pages = (8 * (num_pages * 4096 - 4095) - 7) / (4096 * 8 + 1) + 1;
                size_t bitmap_size = (num_pages + 7) / 8;
                
                pmm_area_t *area = (void*)(to_vaddr(base));
               
                area->free_idx_hint = 0;
                area->bitmap.size = num_free_pages;
                area->num_pages = num_free_pages;
                area->addr = base + align_up(sizeof(pmm_area_t) + bitmap_size, PAGE_SIZE);
                area->next = area_list;

                pmm_free_num_pages += num_free_pages;
                memset(area->bitmap.map, 0, bitmap_size);

                area_list = area;
            } else {
            }
        }
    }

    info("Initialized!"); 
}

paddr_t pmm_palloc(size_t num_pages)
{
    if (num_pages == 0) return 0;

    pmm_area_t *curr = area_list;
    while (curr)
    {
        if (curr->num_pages >= num_pages)
        {
            size_t idx = bitmap_find_contiguous_range_from(&curr->bitmap, curr->free_idx_hint, num_pages);
            if (idx != (size_t)(-1))
            {
                if (idx == curr->free_idx_hint) curr->free_idx_hint += num_pages;
                bitmap_set_range(&curr->bitmap, idx, num_pages); 
                
                curr->num_pages -= num_pages;
                pmm_free_num_pages -= num_pages;

                paddr_t addr = (curr->addr) + idx * PAGE_SIZE;
                return addr;
            }
        }
        curr = curr->next;
    }
    return 0;}

void pmm_pfree(paddr_t addr, size_t num_pages)
{
    pmm_area_t *curr = area_list;
    
    while (curr)
    {
        paddr_t lower_bound = to_paddr(curr);
        paddr_t higher_bound = to_paddr(curr) + sizeof(*curr) + curr->bitmap.size * PAGE_SIZE;   
        if (lower_bound <= addr && addr < higher_bound) break;
        curr = curr->next;
    }
    if (!curr) return;
    
    size_t idx = (addr - (to_paddr(curr->bitmap.map)) - (curr->bitmap.size + 7) / 8) / PAGE_SIZE;
    if (idx < curr->free_idx_hint) curr->free_idx_hint = idx;
    bitmap_unset_range(&curr->bitmap, idx, num_pages);
    pmm_free_num_pages += num_pages;
}

paddr_t pmm_alloc(size_t size)
{
    size_t num_pages = align_up(size, PAGE_SIZE) / PAGE_SIZE;
    return pmm_palloc(num_pages);
}

void pmm_free(paddr_t addr, size_t size)
{
    size_t num_pages = align_up(size, PAGE_SIZE) / PAGE_SIZE;
    pmm_pfree(addr, num_pages);
}
