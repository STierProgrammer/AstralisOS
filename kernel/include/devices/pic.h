#pragma once

#define CMD_MASTER_PORT 0x20
#define DATA_MASTER_PORT 0x21
#define CMD_SLAVE_PORT 0xA0
#define DATA_SLACE_PORT 0xA1

#define CMD_EOI 0x20

#include <stdint.h>

void pic_send_eoi(uint8_t irq);
