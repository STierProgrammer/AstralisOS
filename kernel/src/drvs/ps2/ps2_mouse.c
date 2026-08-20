#include "devs/dev.h"
#include <libds/include/ringbuf.h>
#include <libinput/include/mse.h>
#include "mm/vheap.h"
#include <kernel.h>
#include <stdbool.h>

#ifdef __ARCH_X86_64__
#include <arch/x86_64/ints/pic.h>
#endif

#include <drvs/ps2/ps2.h>
#include <drvs/ps2/ps2_mouse.h>
#include <drvs/ps2/misc.h>

#include <misc/debug.h>

static ringbuf_t evbuf;

static ssize_t ps2_mse_read(void *buf, size_t count)
{
    ssize_t read_count = 0;
    while (!ringbuf_empty(&evbuf) && count--)
    {
        ringbuf_read(&evbuf, (char*)buf + (read_count++) * evbuf.size);
    }
    return read_count;
}

void ps2_mouse_init(void)
{
    ps2_writeCmd(0xD4);
    ps2_writeData(0xF4);

    if (ps2_readData() != PS2_RES_ACK) 
        return;

    krnlctx(interrupt_controller)->clear_mask(12);
    info("Initalized!");

    ringbuf_init(&evbuf, 256, sizeof(mse_ev_t), RINGBUF_MODE_OVERWRITE, vmalloc);

    dev_create("ps2-mse", DEV_MAJOR_INPUT, DEV_MINOR_MSE, NULL, ps2_mse_read);
}

static uint8_t packet_idx = 0;
static uint8_t packet[3] = {0};

void ps2_mouse_callback(void)
{
    int data = ps2_readData();
    if (data < 0) 
        return;
    
    packet[packet_idx++] = data;

    if (packet_idx == 3)
    {
        mse_ev_t ev = {0};
        int dx = (int)packet[1];
        int dy = (int)packet[2];
    
        if (packet[0] & (1 << 4)) dx |= 0xFFFFFF00;
        if (packet[0] & (1 << 5)) dy |= 0xFFFFFF00;
        
        ev.dx = dx;
        ev.dy = dy;
        ev.action |= (packet[0] & (1 << 0)) | (packet[0] & (1 << 1)) | (packet[0] & (1 << 2)); 
            
        ringbuf_write(&evbuf, &ev);

        packet_idx = 0;
    }
}

