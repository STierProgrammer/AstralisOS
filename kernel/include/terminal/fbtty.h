#pragma once

#include <fb.h>
#include <gfx/font.h>
#include <gfx/surface.h>
#include <stdarg.h>

typedef struct fbtty_t {
    gfx_surface_t *surface;
    size_t cursor_x;
    size_t cursor_y;

    const gfx_font_t *font;

    uint32_t foreground;
    uint32_t background;
} fbtty_t;

void fbtty_init(fbtty_t *tty, fb_t *fb);
void fbtty_put(fbtty_t *tty, const char ch);
void fbtty_backspace(fbtty_t *tty);

void fbtty_log(void *priv, const char *fmt, va_list args);

