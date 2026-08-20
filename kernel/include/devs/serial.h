#pragma once

#include <misc/logger.h>

#define COM1_PORT 0x3f8

int serial_init();
int serial_received();
char read_serial();
int is_transmit_empty();

void srput(int a);
void srputs(const char *str);
void srprintf(const char *fmt, ...);
void serial_com1_callback();

extern logger_t serial_logger;

