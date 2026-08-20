#include "misc/debug.h"
#include <stdbool.h>
#include <stdint.h>
#include <arch/x86_64/pit.h>
#include <arch/x86_64/hw/io.h>
#include <arch/x86_64/ints/pic.h>

static volatile uint64_t tick = 0;

void timer_callback(void)
{
    tick++;
}

uint64_t pit_get_current_tick(void)
{
    return tick;
}

void pit_sleep_ms(uint64_t ms)
{
    uint64_t prev_tick = tick;
    while (true)
    {
        uint64_t curr_tick = tick;
        if (curr_tick - prev_tick >= ms) break;
    }
}

void pit_init(uint32_t freq)
{    
    uint32_t div = 1193180 / freq;
    outb(0x43, 0x36);
    uint8_t l = (uint8_t)(div & 0xFF);
    uint8_t h = (uint8_t)((div >> 8) & 0xFF);
    outb(0x40, l);
    outb(0x40, h);
    irq_clear_mask(0);
    info("Initalized!");
}
