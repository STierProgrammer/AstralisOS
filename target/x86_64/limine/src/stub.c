#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "bootstub.h"
#include "fb.h"
#include "modules.h"

static bootloader_ctx_t bootloader_ctx;
static void load_bootloader_ctx(void);
extern void kmain(bootloader_ctx_t *ctx);

#define LIMINE_API_REVISION 4
#include "limine.h"

__attribute__((used, section(".limine_requests")))
static volatile uint64_t limine_base_revision[] = LIMINE_BASE_REVISION(4);

__attribute__((used, section(".limine_requests_start")))
static volatile uint64_t limine_requests_start_marker[] = LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests_end")))
static volatile uint64_t limine_requests_end_marker[] = LIMINE_REQUESTS_END_MARKER;

__attribute__((used, section(".limine_requests"))) 
static volatile struct limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST_ID,
    .revision = 4};

__attribute__((used, section(".limine_requests")))
static volatile struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST_ID,
    .revision = 4};

__attribute__((used, section(".limine_requests"))) 
static volatile struct limine_executable_address_request kernel_address_request = {
    .id = LIMINE_EXECUTABLE_ADDRESS_REQUEST_ID,
    .revision = 4};

__attribute__((used, section(".limine_requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST_ID,
    .revision = 4};

__attribute__((used, section(".limine_requests")))
static volatile struct limine_module_request module_request = {
    .id = LIMINE_MODULE_REQUEST_ID,
    .revision = 4
};

__attribute__((used, section(".limine_requests")))
static volatile struct limine_rsdp_request rsdp_request = {
    .id = LIMINE_RSDP_REQUEST_ID,
    .revision = 4
};

static int from_limine_type(int type) 
{
    switch (type) 
    {
        case LIMINE_MEMMAP_USABLE: return MEMMAP_USABLE;
        case LIMINE_MEMMAP_RESERVED: return MEMMAP_RESERVED;
        case LIMINE_MEMMAP_RESERVED_MAPPED: return MEMMAP_RESERVED_MAPPED;
        case LIMINE_MEMMAP_ACPI_RECLAIMABLE: return MEMMAP_ACPI_RECLAIMABLE;
        case LIMINE_MEMMAP_EXECUTABLE_AND_MODULES: return MEMMAP_EXECUTABLE_AND_MODULES;
        case LIMINE_MEMMAP_ACPI_NVS: return MEMMAP_ACPI_NVS;
        case LIMINE_MEMMAP_BAD_MEMORY: return MEMMAP_BAD_MEMORY;
        case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE: return MEMMAP_BOOTLOADER_RECLAIMABLE;
        case LIMINE_MEMMAP_FRAMEBUFFER: return MEMMAP_FRAMEBUFFER;
        default: return MEMMAP_UNKNOWN;
    }
}

static void get_krnl_map(krnl_map_t *krnl_map)
{
    if (kernel_address_request.response)
    {
        krnl_map->physical_base = kernel_address_request.response->physical_base;
        krnl_map->virtual_base = kernel_address_request.response->virtual_base;
    }
}

static void get_memmap_entry(memmap_t *memmap, memmap_entry_t *entry, size_t i)
{
    if (i >= memmap->num_entries)
        return;

    entry->base = memmap_request.response->entries[i]->base;
    entry->length = memmap_request.response->entries[i]->length;
    entry->type = from_limine_type(memmap_request.response->entries[i]->type);
}

static void get_memmap(memmap_t *memmap)
{
    if (memmap_request.response)
    {
        memmap->num_entries = memmap_request.response->entry_count;
        memmap->get_entry = get_memmap_entry;
    }
}

void get_fb(fbs_t *fbs, fb_t *fb, size_t i)
{
    if (i >= fbs->num_fbs)
        return;
    struct limine_framebuffer *lfb = framebuffer_request.response->framebuffers[i];
    fb->addr = (uint64_t)lfb->address;
    fb->width = lfb->width;
    fb->height = lfb->height;
    fb->bpp = lfb->bpp;
    fb->pitch = lfb->pitch;
    fb->memory_model = lfb->memory_model;
    fb->blue_mask_shift = lfb->blue_mask_shift;
    fb->red_mask_shift = lfb->red_mask_shift;
    fb->green_mask_shift = lfb->green_mask_shift;
    fb->red_mask_size = lfb->red_mask_size;
    fb->blue_mask_size = lfb->blue_mask_size;
    fb->green_mask_size = lfb->green_mask_size;
}

static void get_fbs(fbs_t *fbs)
{
    fbs->num_fbs = framebuffer_request.response->framebuffer_count;
    fbs->get_fb = get_fb;
}

static inline uint64_t get_modules_count(void)
{
    if (module_request.response) return module_request.response->module_count;
    return 0;
}

static void get_module(modules_t *mods, module_t *mod, size_t i)
{
    if (i >= mods->num_modules)
        return;
    
    struct limine_file *file = module_request.response->modules[i];
    mod->addr = file->address;
    mod->path = file->path;
    mod->size = file->size;
}

static void get_modules(modules_t *modules)
{
    modules->get_module = get_module;
    uint64_t modules_count = get_modules_count();
    if (modules_count == 0) 
        return;
    
    modules->num_modules = modules_count;
}

static void get_rsdp(acpi_rsdp_t *rsdp)
{
    struct limine_rsdp_response *res = rsdp_request.response;
    if (res)
    {
        rsdp->addr = (uint64_t)res->address;
        rsdp->revision = res->revision;
    }
}

static void load_bootloader_ctx(void)
{
    get_krnl_map(&bootloader_ctx.krnl_map);
    get_memmap(&bootloader_ctx.memmap);
    get_fbs(&bootloader_ctx.fbs);
    get_rsdp(&bootloader_ctx.acpi_rsdp);
    get_modules(&bootloader_ctx.modules);
    
    bootloader_ctx.hhdm = hhdm_request.response->offset;
}

void kstart(void) 
{
    load_bootloader_ctx();
    kmain(&bootloader_ctx);
}
