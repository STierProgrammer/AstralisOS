#include <kernel.h>

#include <misc/debug.h>

#include <arch/x86_64/ints/pic.h>
#include <arch/x86_64/hw/io.h>

static const int_ctrl_t pic;

void pic_send_eoi(uint8_t irq)
{
    // TODO: Check if I have to shift the irq number
    if (irq >= 8) outb(PIC2_CMD, PIC_EOI);
    outb(PIC1_CMD, PIC_EOI);
    io_wait();
}

void pic_init() {
    outb(PIC1, 0x11);
    outb(PIC2, 0x11);

    outb(PIC1_DATA, 0x20);
    outb(PIC2_DATA, 0x28);

    outb(PIC1_DATA, 0x04);
    outb(PIC2_DATA, 0x02);

    outb(PIC1_DATA, 0b1);
    outb(PIC2_DATA, 0b1);

    outb(PIC1_DATA, 0b11111111);
    outb(PIC2_DATA, 0b11111111);

    krnlctx(interrupt_controller) = &pic;
    info("Initalized!");
}

void pic_disable(void) {
    outb(PIC1_DATA, 0xff);
    outb(PIC2_DATA, 0xff);
}

void irq_set_mask(uint8_t IRQline) {
    uint16_t port;
    uint8_t value;

    if(IRQline < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        IRQline -= 8;
    }
    value = inb(port) | (1 << IRQline);
    outb(port, value);        
}

void irq_clear_mask(uint8_t IRQline) {
    uint16_t port;
    uint8_t value;

    if(IRQline < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        IRQline -= 8;
    }
    value = inb(port) & ~(1 << IRQline);
    outb(port, value);        
}

static const int_ctrl_t pic = {
    .name = "PIC",
    .uninit = pic_disable,
    .send_eoi = pic_send_eoi,
    .set_mask = irq_set_mask,
    .clear_mask = irq_clear_mask,
};
