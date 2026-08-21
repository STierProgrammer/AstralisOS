#include <gfx/font.h>
#include <gfx/surface.h>
#include <terminal/fbtty.h>
#include <stdarg.h>
#include <misc/printf.h>
#include <misc/debug.h>

#define fbtty_width(tty) ((tty)->surface->framebuffer->width)

#define SPACE_SIZE 1
#define LINE_SPACING 1
#define DEFAULT_FOREGROUND_COLOR 0x27CFF5

void fbtty_init(fbtty_t *tty, fb_t *fb)
{
    tty->foreground = DEFAULT_FOREGROUND_COLOR;
    tty->background = 0;
    tty->surface = gfx_surface_create(fb);
    tty->cursor_x = 0;
    tty->cursor_y = 0;
    tty->font = &iso_font;
}

#define BACKGROUND_COLOR 0x0000

void fbtty_backspace(fbtty_t *tty)
{
    if (tty->cursor_x == 0 && tty->cursor_y == 0) return;

    if (tty->cursor_x == 0)
    {
        tty->cursor_y -= tty->font->height + SPACE_SIZE;
        tty->cursor_x = fbtty_width(tty) - (tty->font->width + SPACE_SIZE);
    }
    else
    {
        tty->cursor_x -= tty->font->width + SPACE_SIZE;
    }

    gfx_draw_rect(tty->surface, tty->cursor_x, tty->cursor_y, 
                  tty->font->width + SPACE_SIZE, tty->font->height, 
                  BACKGROUND_COLOR);

    gfx_surface_sync(tty->surface, tty->cursor_x + tty->cursor_y * fbtty_width(tty),
                     tty->cursor_x + fbtty_width(tty) * (tty->cursor_y + tty->font->height));
}

static inline void fbtty_newline(fbtty_t *tty)
{
    tty->cursor_x = 0;
    tty->cursor_y += tty->font->height + LINE_SPACING;
}

void fbtty_put(fbtty_t *tty, const char ch)
{
    if (ch == '\n')
    {
        fbtty_newline(tty);
        return;
    }

    if (tty->cursor_x + tty->font->width >= fbtty_width(tty))
    {
        fbtty_newline(tty);
    }

    size_t prev_cursor_x = tty->cursor_x;
    gfx_draw_char(tty->surface, ch, tty->font, tty->cursor_x, tty->cursor_y, tty->foreground);
    tty->cursor_x += tty->font->width + SPACE_SIZE;
    gfx_surface_sync(tty->surface, prev_cursor_x + tty->cursor_y * fbtty_width(tty), tty->cursor_x + fbtty_width(tty) * (tty->cursor_y + tty->font->height));
}

void fbtty_puts(fbtty_t *tty, const char *str)
{
    if (!str) 
        return;

    const char *s = str;
    while (*s)
    {
        fbtty_put(tty, *s);
        s++;
    }
}

static void logger_put(void *priv, int c)
{
    fbtty_put((fbtty_t*)priv, c);
}

static void logger_puts(void *priv, const char *str)
{
    fbtty_puts((fbtty_t*)priv, str);
}

void fbtty_log(void *priv, const char *fmt, va_list args)
{
    fbtty_t *fbtty = (fbtty_t*)priv;
    _printf(fbtty, logger_put, logger_puts, fmt, args);
}

