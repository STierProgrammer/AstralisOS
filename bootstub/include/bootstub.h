#ifndef OPEN95_BOOTSTUB_H
#define OPEN95_BOOTSTUB_H

#include <stdint.h>
#include <stddef.h>

enum {
    MEMMAP_RESERVED,
    MEMMAP_ACPI_RECLAIMABLE,
    MEMMAP_EXECUTABLE_AND_MODULES,
    MEMMAP_ACPI_NVS,
    MEMMAP_BAD_MEMORY,
    MEMMAP_BOOTLOADER_RECLAIMABLE,
    MEMMAP_FRAMEBUFFER,
    MEMMAP_USABLE,
    MEMMAP_UNKNOWN
};

typedef struct memmap_entry_t {
    uint64_t base;
    uint64_t length;
    uint64_t type;
} memmap_entry_t;

typedef struct memmap_t {
    memmap_entry_t* entries;
    size_t entry_count;
} memmap_t;

typedef struct kernel_addr_t {
    uint64_t physical_base;
    uint64_t virtual_base;
} kernel_addr_t;

typedef struct framebuffer_t {
    uint64_t addr;
    uint64_t width;
    uint64_t height;
    uint64_t pitch;
    uint8_t memory_model;
} framebuffer_t;

typedef struct kernel_params_t 
{
    uint64_t hhdm;
    kernel_addr_t kernel_addr;
    memmap_t memmap;
    framebuffer_t *framebuffers;
} kernel_params_t;

#endif