#include <stddef.h>
#include <stdint.h>
#include <fs/vfs.h>
#include <fs/initrd.h>
#include <arch/x86_64/pit.h>
#include <gfx/surface.h>
#include <misc/debug.h>
#include <terminal/fbtty.h>

static fbtty_t fbtty;

#define WIDTH  (16 * 16 * 2)
#define HEIGHT (9  * 16 * 2)
#define SCALE  2

enum {
    // Sets cursor to 0, 0
    CMD_RESET = 0x0,
    // 5 bit
    CMD_MEMSET0 = 0x1,
    // 8 + 5 bit
    CMD_MEMSET1 = 0x2,
    // 5 bit
    CMD_RANGE0 = 0x3,
    // 8 + 5 bit
    CMD_RANGE1 = 0x4,
    // 5 bit
    CMD_SKIP0 = 0x5,
    // 8 + 5 bit
    CMD_SKIP1 = 0x6,
};

uint32_t color_table[256] = {
    0x5b43a7, 0x020004, 0x1d1425, 0x535253, 0x5c469f, 0x17054f, 0x060007, 0x7e6897, 0x7477c5, 0x251c3c, 0x8a6bb8, 0x7e6ac0, 0x110002, 0x1d0f12, 0x1b181a, 0x545255, 
    0x8c72b1, 0x876cac, 0x856cd6, 0x342a45, 0xf6f4f6, 0x050306, 0x090207, 0xedf0fe, 0x565257, 0x0a0104, 0x9a6269, 0x160649, 0x735893, 0x765bd1, 0x8e71b7, 0x565bab, 
    0x5e4974, 0x2a282b, 0x533c9c, 0x7a6197, 0x110b18, 0x4c4a4c, 0x856ed0, 0x5a46a5, 0x090507, 0xcfcdd1, 0x2f2439, 0xece9eb, 0x995c63, 0x885259, 0x755b95, 0x5c595e, 
    0x76608b, 0xa56c73, 0xd5d4d5, 0x817f82, 0x8e8c8d, 0x57426d, 0xdad7db, 0x060000, 0x19111d, 0x151216, 0xf3f1f3, 0x0b0515, 0x846adf, 0x645fb0, 0x787678, 0x020006, 
    0x00000b, 0x7a5d96, 0x170450, 0x131010, 0x67636a, 0x5b437b, 0x595eac, 0x17090b, 0x211c35, 0xb6b3b5, 0x826fca, 0xa6abf4, 0x000007, 0x020100, 0x585557, 0xb1afb1, 
    0x231c2c, 0x8970d8, 0x12070a, 0x814950, 0x595aa7, 0x332644, 0x070001, 0x6048b5, 0x1b142d, 0x9f5a62, 0x4e4c4e, 0x949194, 0x000010, 0x030008, 0x291e41, 0x735db8, 
    0x382750, 0x836ad3, 0xcac8c9, 0x474547, 0x26143d, 0x252326, 0x020019, 0x595ba2, 0x6b52b9, 0x555ca7, 0x080709, 0x945b62, 0x010000, 0x81619a, 0x040003, 0x141312, 
    0xfefcfd, 0x694d8d, 0x7e5459, 0x644ea8, 0x3c328c, 0xdfdddf, 0x080012, 0x200d27, 0x331a4e, 0x898788, 0xe9e7e9, 0x13014c, 0xacaaab, 0xbebcbe, 0xa9a4a8, 0x7e7b7d, 
    0x1e0e2e, 0x4b1f25, 0x12080b, 0x715e9a, 0x8c8492, 0x9b5e65, 0x513e93, 0x844d53, 0x0b080b, 0x614782, 0x91535a, 0x030202, 0x010002, 0x000002, 0xe6e4e6, 0x634e7d, 
    0x080807, 0xbab7b8, 0xbfb1c9, 0x422f51, 0x3b3141, 0xfaf8fb, 0x010022, 0x8068cf, 0x5f47ad, 0x77589b, 0x040205, 0x5948a5, 0x140207, 0xeeecee, 0x535aa7, 0x8f5e64, 
    0x160a0c, 0x8368ae, 0x534879, 0x826ba5, 0x50539e, 0x684eba, 0x140a0d, 0x120f11, 0x2d1a36, 0xeae8ea, 0x0d0510, 0x9071be, 0x55487d, 0x8d75ab, 0x383639, 0x0f0314, 
    0x4f3a6a, 0x715a90, 0x7c62a0, 0x535058, 0x000004, 0x222023, 0x7b61d9, 0x5958a8, 0x312e30, 0x3f3d3e, 0x755dc2, 0x242524, 0xa0646b, 0x1f1d1f, 0x9c5960, 0x0e0a0e, 
    0x31284a, 0x140508, 0x2d1841, 0x21135e, 0x00003f, 0x858386, 0x686db8, 0x6149af, 0x0f0d11, 0xc6c4c6, 0x020001, 0x090000, 0x190d22, 0x0f0015, 0x2d2e79, 0x615f61, 
    0x32203a, 0x93555c, 0x10013e, 0x7f69c7, 0x181517, 0xc3c0c2, 0x454241, 0x030000, 0x795f9b, 0x716d68, 0x795da3, 0xa19ea0, 0xe1dafd, 0x5b49a6, 0x976066, 0x695485, 
    0x5b43ac, 0x0c0305, 0x211731, 0x180d1b, 0xe9e6e9, 0x0c0b0b, 0xebebe9, 0x999799, 0x1e0c0f, 0xdddade, 0x755b92, 0xa3676e, 0x351f49, 0x806eb8, 0xc6becc, 0x6c6a6c, 
    0x737073, 0x4a3560, 0x07000a, 0x272138, 0x000000, 0x0b030e, 0x231136, 0x7c609b, 0x160f16, 0xf1eff0, 0xe5e2e3, 0x795992, 0x483f51, 0x605b9c, 0x11031f, 0x623339
};

typedef long ssize_t;

void bad_apple()
{
    inode_t *bad_apple = NULL;
    initrd_get("badapple.raw", &bad_apple);
    if (bad_apple)
    {
        debug("Found bad apple");
    }

    static uint8_t cmds[1024*1024*8];
    ssize_t count = inode_read(bad_apple, cmds, sizeof(cmds), 0);
    if(count < 0) {
        debug("Fuck");
        return;
    }

    static uint8_t frames[WIDTH * HEIGHT];
    size_t off_x = (fbtty.surface->framebuffer->width - WIDTH * SCALE) / 2;
    size_t off_y = (fbtty.surface->framebuffer->height - HEIGHT * SCALE) / 2;
    uint8_t *head = cmds;
    for(;;) {
        long starting = pit_get_current_tick();

        size_t cursor = 0;
        while(head < cmds + count) {
            uint8_t cmd = *head++;
            uint8_t op = cmd & 0x7;
            uint8_t data0 = (cmd >> 3) & 0x1F;

            if(op == CMD_RESET) break;
            size_t n = data0;
            if((op & 1) == 0) {
                // assert(head < cmds + size);
                n |= (*head++) << 5;
            }

            // assert(WIDTH*HEIGHT - cursor >= n);
            switch(op) {
            case CMD_MEMSET1:
            case CMD_MEMSET0: {
                // assert(cmds + size - head > 0);
                uint8_t byte = *head++;
                memset(frames + cursor, byte, n);
            } break;
            case CMD_RANGE1:
            case CMD_RANGE0: {
                // assert(cmds + size - head >= n);
                memcpy(frames + cursor, head, n);
                head += n;
            } break;
            case CMD_SKIP1:
            case CMD_SKIP0:
                break;
            }
            cursor += n;
        }
        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                int index = y * WIDTH + x;
                uint32_t color = color_table[frames[index]];
                gfx_draw_rect(fbtty.surface, off_x + x * SCALE, off_y + y * SCALE, SCALE, SCALE, color);
            }
        }
        gfx_surface_sync_chunk(fbtty.surface, off_x, off_y, WIDTH * SCALE, HEIGHT * SCALE);
        long ending = pit_get_current_tick();   
        long delta = (ending - starting);
        if (delta < 33)
        {   
            pit_sleep_ms(33 - delta);
        }
    }
}


