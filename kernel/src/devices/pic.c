#include "devices/pic.h"

#if defined(__x86_64__) || defined(_M_X64)
#include "arch/x86_64/def.h"
#endif

void pic_send_eoi(uint8_t irq)
{
    if (irq >= 8) outb(CMD_SLAVE_PORT, CMD_EOI);
    outb(CMD_MASTER_PORT, CMD_EOI);
}