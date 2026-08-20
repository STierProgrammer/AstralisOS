#include <kernel.h>
#include <mm/vheap.h>
#include <stdbool.h>
#include <stdint.h>
#include <libds/include/ringbuf.h>

#ifdef __ARCH_X86_64__
#include <arch/x86_64/ints/pic.h>
#endif

#include <libinput/include/kbd.h>
#include <drvs/ps2/misc.h>
#include <drvs/ps2/kbd/scancodes.h>
#include <drvs/ps2/kbd/misc.h>
#include <misc/debug.h>
#include <misc/helpers.h>
#include <devs/dev.h>

static uint8_t current_scancode_set;
static ringbuf_t key_ringbuf;

static ssize_t ps2_kbd_read(void *buf, size_t count)
{
    ssize_t read_count = 0;
    while (!ringbuf_empty(&key_ringbuf) && count--)
    { 
        read_count += 1;
        ringbuf_read(&key_ringbuf, buf);
    }
    return read_count;
}

void ps2_keyboard_init(void)
{
    current_scancode_set = ps2_keyboard_get_scancode_set(); 
    krnlctx(interrupt_controller)->clear_mask(1);
    info("Initalized!");
    ringbuf_init(&key_ringbuf, 256, sizeof(kbd_ev_t), RINGBUF_MODE_OVERWRITE, vmalloc); 

    dev_create("ps2-kbd", DEV_MAJOR_INPUT, DEV_MINOR_KBD, NULL, ps2_kbd_read);
}

void ps2_keyboard_callback(void)
{   
    int sc = ps2_readData();
    if (sc > 0xFF) return;

    kbd_ev_t ev = ps2_decodeFromScancode(sc, current_scancode_set);
    if (ev.keycode != KEY_NONE)
    { 
        ringbuf_write(&key_ringbuf, &ev);
    }
}



