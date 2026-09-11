#pragma once

#include <stdint.h>
#include <stddef.h>

typedef uint32_t color_t;

typedef struct
{
    color_t* addr;
    size_t width;
    size_t height;
    size_t pitch;

    uint8_t memory_model;
    uint16_t bpp;
    
    uint8_t red_mask_size;
    uint8_t red_mask_shift;
    
    uint8_t green_mask_size;
    uint8_t green_mask_shift;
    
    uint8_t blue_mask_size;
    uint8_t blue_mask_shift;
} fb_t;

typedef struct fbs_t fbs_t;
typedef struct fbs_t {
    void (*get_fb)(fbs_t *fbs, fb_t *fb, size_t idx);
    size_t num_fbs;
} fbs_t;


static inline color_t colorFromRGB(fb_t *fb, uint8_t r, uint8_t g, uint8_t b)
{
    uint32_t color = 0;
    color |= (r * (fb->red_mask_size) / 255) << fb->red_mask_shift;
    color |= (g * (fb->green_mask_size) / 255) << fb->green_mask_shift;
    color |= (b * (fb->blue_mask_size) / 255) << fb->blue_mask_shift;
    return color;
}

