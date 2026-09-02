#pragma once

#include <kernel.h>
#include <stdint.h>

#define PAGE_FLAG_PRESENT           1 << 0
#define PAGE_FLAG_READ_WRITE        1 << 1
#define PAGE_FLAG_USER_SUPERVISOR   1 << 2
#define PAGE_FLAG_WRITE_THROUGH     1 << 3
#define PAGE_FLAG_CACHE_DISABLE     1 << 4
#define PAGE_FLAG_ACCESSED          1 << 5
#define PAGE_FLAG_DIRTY             1 << 6
#define PAGE_FLAG_PAT               1 << 7
#define PAGE_FLAG_GLOBAL            1 << 8
#define PAGE_FLAG_COW               1 << 9
#define PAGE_FLAG_EXEC              1UL << 63

typedef uint64_t page_flags_t;
typedef uint64_t page_entry_t;
typedef struct page_table_t {
    page_entry_t entries[512];
} page_table_t;

#define PAGE_PHYSICAL_ADDRESS_MASK  0xfffffffff000

void          pt_unmap(page_table_t *pt, vaddr_t vaddr);
void          pt_map(page_table_t *pt, paddr_t paddr, vaddr_t vaddr, page_flags_t flags);
page_table_t *pt_create(void);
void          pt_swap(page_table_t *pt);
void          pt_map_kernel(page_table_t *pt);
void          pt_map_memmap(page_table_t *pt);
void          pt_join_kernel(page_table_t *user_pt, page_table_t *kernel_pt);


