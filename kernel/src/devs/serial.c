#include "misc/debug.h"
#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>

#ifdef __ARCH_X86_64__
#include <arch/x86_64/hw/io.h>
#endif

#include <misc/printf.h>
#include <misc/logger.h>
#include <devs/serial.h>

logger_t serial_logger;

int serial_init()
{
    outb(COM1_PORT + 1, 0x00); // Disable all interrupts
    outb(COM1_PORT + 3, 0x80); // Enable DLAB (set baud rate divisor)
    outb(COM1_PORT + 0, 0x03); // Set divisor to 3 (lo byte) 38400 baud
    outb(COM1_PORT + 1, 0x00); //                  (hi byte)
    outb(COM1_PORT + 3, 0x03); // 8 bits, no parity, one stop bit
    outb(COM1_PORT + 2, 0xC7); // Enable FIFO, clear them, with 14-byte threshold
    outb(COM1_PORT + 4, 0x0B); // IRQs enabled, RTS/DSR set
    outb(COM1_PORT + 4, 0x1E); // Set in loopback mode, test the serial chip
    outb(COM1_PORT + 0, 0xAE); // Test serial chip (send byte 0xAE and check if serial returns same byte)

    // Check if serial is faulty (i.e: not same byte as sent)
    if (inb(COM1_PORT + 0) != 0xAE)
    {
        return 1;
    }

    // If serial is not faulty set it in normal operation mode
    // (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled)
    outb(COM1_PORT + 4, 0x0F);

    logger_register(&serial_logger);
    
    return 0;
}

int serial_received()
{
    return inb(COM1_PORT + 5) & 1;
}

char read_serial()
{
    while (serial_received() == 0)
        ;
    return inb(COM1_PORT);
}

int is_transmit_empty()
{
    return inb(COM1_PORT + 5) & 0x20;
}

void srput(int a)
{
    while (is_transmit_empty() == 0)
        ;

    outb(COM1_PORT, a);
}

void srputs(const char *str)
{
    while (*str)
    {
        srput(*str);
        str++;
    }
}

static void _srput(void *priv, int a)
{
    (void)priv;
    srput(a);
}

static void _srputs(void *priv, const char *str)
{
    (void)priv;
    srputs(str);
}

// TODO: Add other specifiers
void srprintf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
 
    _printf(NULL, _srput, _srputs, fmt, args);

    va_end(args);
}

void serial_com1_callback()
{
    srdebug(serial_com1_callback, "Called!");
}

static void serial_log(void *priv, const char *fmt, va_list args)
{
    (void)priv;
    _printf(NULL, _srput, _srputs, fmt, args);
}

logger_t serial_logger = {
    .name = "Serial Logger",
    .log = serial_log
};

