#pragma once

#define COM1_PORT 0x3f8

int serial_init();
int serial_received();
char read_serial();
int is_transmit_empty();

void srput(int a);
