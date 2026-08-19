#include "kernel.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "misc/debug.h"
#include <bootstub.h>

#include <arch/x86_64/mm/paging.h>
#include <arch/x86_64/cpu/cpu.h>

#include <misc/helpers.h>
#include <mm/pmm/pmm.h>

extern krnl_ctx_t krnl_ctx;

page_table_t* pt_create(void)
{
    uint64_t pt_paddr = pmm_palloc(1);
    page_table_t *pt = paddr_ptr(pt_paddr);
    memset(pt->entries, 0, PAGE_SIZE);
    return pt;
}

void pt_swap(page_table_t *pt)
{
    write_cr3(to_paddr((vaddr_t)pt));
}

static void set_page_entry(page_entry_t *entry)
{
    if (!(*entry & PAGE_FLAG_PRESENT))
    {
        *entry = 0;
        *entry |= PAGE_FLAG_PRESENT;
        *entry |= pmm_palloc(1) & PAGE_PHYSICAL_ADDRESS_MASK;
        memset((void*)to_vaddr((*entry & PAGE_PHYSICAL_ADDRESS_MASK)), 0, PAGE_SIZE);
    }
}

static void pt_map_all(page_table_t *pt, vaddr_t start, vaddr_t end, paddr_t at, page_flags_t flags)
{
    size_t pages = (end - start) / PAGE_SIZE;
    for (size_t i = 0; i < pages; ++i)
        pt_map(pt, at + i * PAGE_SIZE, start + i * PAGE_SIZE, flags);
}

static void map_kernel_sections_to_pt(page_table_t *pt, vaddr_t sec_start, vaddr_t sec_end, page_flags_t flags)
{
    krnl_map_t map = bootctx(krnl_map);
    size_t offset = sec_start - (vaddr_t)map.virtual_base;
    paddr_t kernel_paddr = map.physical_base;

    pt_map_all(pt, sec_start, sec_end, kernel_paddr + offset, flags);
}

extern char text_start[];
extern char text_end[];

extern char rodata_start[];
extern char rodata_end[];

extern char mut_data_start[];
extern char mut_data_end[];

vaddr_t hhdm_end = 0;

void pt_join_kernel(page_table_t *user_pt, page_table_t *kernel_pt)
{
    for (int i = 256; i < 512; ++i)
    {
        user_pt->entries[i] = kernel_pt->entries[i];
    }
}

void pt_map_kernel(page_table_t *pt)
{
    map_kernel_sections_to_pt(pt, (vaddr_t)text_start, (vaddr_t)text_end, PAGE_FLAG_PRESENT);
    map_kernel_sections_to_pt(pt, (vaddr_t)rodata_start, (vaddr_t)rodata_end, PAGE_FLAG_PRESENT);
    map_kernel_sections_to_pt(pt, (vaddr_t)mut_data_start, (vaddr_t)mut_data_end, PAGE_FLAG_PRESENT | PAGE_FLAG_READ_WRITE);
}

void pt_map_memmap(page_table_t *pt)
{
    for (size_t i = 0; i < krnl_ctx.bootloader_ctx->memmap.num_entries; i++)
    {
        memmap_entry_t entry;
        bootctx(memmap).get_entry(&bootctx(memmap), &entry, i);
        switch (entry.type) {
            case MEMMAP_FRAMEBUFFER:
            case MEMMAP_USABLE:
            case MEMMAP_BOOTLOADER_RECLAIMABLE:
            case MEMMAP_ACPI_NVS:
            case MEMMAP_EXECUTABLE_AND_MODULES:
            case MEMMAP_ACPI_RECLAIMABLE:
            case MEMMAP_RESERVED_MAPPED:
            {
                uint64_t base = align_up(entry.base, PAGE_SIZE);
                size_t len = align_down(entry.length, PAGE_SIZE);
                vaddr_t end = to_vaddr(base + len);
                page_flags_t flags = PAGE_FLAG_PRESENT | PAGE_FLAG_READ_WRITE;

                if (entry.type == MEMMAP_FRAMEBUFFER) 
                    flags |= PAGE_FLAG_PAT | PAGE_FLAG_WRITE_THROUGH;

                pt_map_all(pt, to_vaddr(base), to_vaddr(base) + len, base, flags);
                if (end > hhdm_end)
                    hhdm_end = end;
            }
        }
    }
}

void pt_map(page_table_t *pt, paddr_t paddr, vaddr_t vaddr, page_flags_t flags)
{
    size_t pt4_idx = (vaddr >> 39) & 0x1FF;
    size_t pt3_idx = (vaddr >> 30) & 0x1FF;
    size_t pt2_idx = (vaddr >> 21) & 0x1FF;
    size_t pt1_idx = (vaddr >> 12) & 0x1FF;
    
    set_page_entry(&pt->entries[pt4_idx]);
    pt->entries[pt4_idx] |= (flags & (PAGE_FLAG_USER_SUPERVISOR | PAGE_FLAG_READ_WRITE));

    page_table_t *pt3 = paddr_ptr(pt->entries[pt4_idx] & PAGE_PHYSICAL_ADDRESS_MASK);
    set_page_entry(&pt3->entries[pt3_idx]);
    pt3->entries[pt3_idx] |= (flags & (PAGE_FLAG_USER_SUPERVISOR | PAGE_FLAG_READ_WRITE));

    page_table_t *pt2 = paddr_ptr(pt3->entries[pt3_idx] & PAGE_PHYSICAL_ADDRESS_MASK);
    set_page_entry(&pt2->entries[pt2_idx]);
    pt2->entries[pt2_idx] |= (flags & (PAGE_FLAG_USER_SUPERVISOR | PAGE_FLAG_READ_WRITE));

    page_table_t *pt1 = paddr_ptr(pt2->entries[pt2_idx] & PAGE_PHYSICAL_ADDRESS_MASK);
    pt1->entries[pt1_idx] = (paddr & PAGE_PHYSICAL_ADDRESS_MASK) | flags;
}


void pt_unmap(page_table_t *pt, vaddr_t vaddr)
{
    size_t pt4_idx = (vaddr >> 39) & 0x1FF;
    size_t pt3_idx = (vaddr >> 30) & 0x1FF;
    size_t pt2_idx = (vaddr >> 21) & 0x1FF;
    size_t pt1_idx = (vaddr >> 12) & 0x1FF;
    
    page_table_t *pt3 = paddr_ptr(pt->entries[pt4_idx] & PAGE_PHYSICAL_ADDRESS_MASK);
    page_table_t *pt2 = paddr_ptr(pt3->entries[pt3_idx] & PAGE_PHYSICAL_ADDRESS_MASK);
    page_table_t *pt1 = paddr_ptr(pt2->entries[pt2_idx] & PAGE_PHYSICAL_ADDRESS_MASK);
    
    pmm_free(pt1->entries[pt1_idx] & PAGE_PHYSICAL_ADDRESS_MASK, PAGE_SIZE / PAGE_SIZE);
    
    pt1->entries[pt1_idx] = 0;
    invplg((uint64_t)pt); 
}
