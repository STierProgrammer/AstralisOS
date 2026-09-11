#include "bootstub.h"
#include "fb.h"

#include "devs/serial.h"

void kmain(bootctx_t *ctx)
{
    serial_init();

    srput('c');
    fb_t fb;
    ctx->fbs.get_fb(&ctx->fbs, &fb, 0);

    for (size_t x = 0; x < fb.width; x++)
    {
        fb.addr[x] = colorFromRGB(&fb, 128, 128, 128);
    }

    for (;;)
        ;
}
