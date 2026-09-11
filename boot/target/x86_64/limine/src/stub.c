#include <stddef.h>
#include <stdint.h>

#include "bootstub.h"
#include "fb.h"

static bootctx_t bootctx;
static void load_bootloader_ctx(void);

extern void kmain(bootctx_t *ctx);

#define LIMINE_API_REVISION 4
#include "limine.h"

__attribute__((used, section(".limine_requests")))
static volatile uint64_t limine_base_revision[] = LIMINE_BASE_REVISION(4);

__attribute__((used, section(".limine_requests_start")))
static volatile uint64_t limine_requests_start_marker[] = LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests_end")))
static volatile uint64_t limine_requests_end_marker[] = LIMINE_REQUESTS_END_MARKER;

__attribute__((used, section(".limine_requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST_ID,
    .revision = 4};

void get_fb(fbs_t *fbs, fb_t *fb, size_t i)
{
    if (i >= fbs->num_fbs)
        return;
    struct limine_framebuffer *lfb = framebuffer_request.response->framebuffers[i];
    fb->addr = lfb->address;
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
static void load_bootloader_ctx(void)
{
    get_fbs(&bootctx.fbs);
}

void kstart(void) 
{
    load_bootloader_ctx();
    kmain(&bootctx);
}
