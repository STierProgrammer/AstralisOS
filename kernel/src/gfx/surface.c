#include "mm/vheap.h"
#include <stdint.h>
#include <string.h>
#ifdef __ARCH_X86_64__
#include <arch/x86_64/mm/paging.h>
#endif

#include <kernel.h>
#include <gfx/surface.h>
#include <mm/vmm/misc.h>

#define surface_backbuffer_idx(surface, idx)       ((surface)->backbuffer + idx)
#define surface_framebuffer_idx(surface, idx)      (((uint32_t*)(surface)->framebuffer->addr) + idx)
#define surface_pixels_count(surface) ((surface)->framebuffer->height * (surface)->framebuffer->width)

gfx_surface_t *gfx_surface_create(fb_t *framebuffer)
{
    gfx_surface_t *surface = vmalloc(sizeof(gfx_surface_t));
    surface->framebuffer = framebuffer;
    surface->backbuffer = (uint32_t*)vmalloc(sizeof(uint32_t) * framebuffer->height * (framebuffer->pitch / 4));
    return surface;
}

void gfx_surface_sync(gfx_surface_t *surface, size_t start, size_t end)
{
    if (start < surface_pixels_count(surface) && end < surface_pixels_count(surface))
    {
        memcpy(surface_framebuffer_idx(surface, start), surface_backbuffer_idx(surface, start), (end - start) * sizeof(uint32_t));
    }
}

void gfx_surface_put_pixel(gfx_surface_t *surface, size_t x, size_t y, uint32_t color)
{
    if (x < surface->framebuffer->width && y < surface->framebuffer->height)
    {
        surface->backbuffer[y * (surface->framebuffer->pitch / 4) + x] = color;
    }
}

void gfx_surface_sync_chunk(
    gfx_surface_t *surface,
    int x,
    int y,
    size_t width,
    size_t height
) {
    size_t screen_w = surface->framebuffer->width;
    size_t screen_h = surface->framebuffer->height;

    int x0 = x;
    int y0 = y;
    int x1 = x + (int)width;
    int y1 = y + (int)height;

    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;

    if (x1 > (int)screen_w) x1 = (int)screen_w;
    if (y1 > (int)screen_h) y1 = (int)screen_h;

    if (x0 >= x1 || y0 >= y1)
        return;

    size_t clipped_width = x1 - x0;
    size_t clipped_height = y1 - y0;

    for (size_t i = 0; i < clipped_height; ++i)
    {
        size_t idx =
            (y0 + i) * surface->framebuffer->pitch / 4 + x0;

        memcpy(
            (uint32_t*)surface->framebuffer->addr + idx,
            surface->backbuffer + idx,
            clipped_width * sizeof(uint32_t)
        );
    }
}

void gfx_draw_rect(gfx_surface_t *surface, int x, int y, size_t width, size_t height, uint32_t color)
{
    uint32_t *line = surface->backbuffer + x + y * surface->framebuffer->pitch / 4;
    for (int i = 0; i < (int)height; ++i)
    {
        for (int j = 0; j < (int)width; ++j)
        {
            line[j] = color;
        }
        line += surface->framebuffer->pitch / 4;
    }
}
