#include "arch/x86_64/desc/tss.h"
#include <arch/x86_64/desc/gdt.h>
#include <misc/debug.h>

gdt_t gdt = {
    .null_descriptor = {
        .low_limit = 0,
        .low_base = 0,
        .mid_base = 0,
        .access_byte = 0,
        .limit_and_flags = 0,
        .high_base = 0,
    },
    .kernel_code_segment = {
        .low_limit = 0xFFFF,
        .low_base = 0,
        .mid_base = 0,
        .access_byte = 0x9A,
        .limit_and_flags = 0x20,
        .high_base = 0,
    },
    .kernel_data_segment = {
        .low_limit = 0xFFFF,
        .low_base = 0,
        .mid_base = 0,
        .access_byte = 0x92,
        .limit_and_flags = 0x20,
        .high_base = 0,
    },
    .user_code_segment = {
        .low_limit = 0xFFFF,
        .low_base = 0,
        .mid_base = 0,
        .access_byte = 0xFA,
        .limit_and_flags = 0x20,
        .high_base = 0,
    },
    .user_data_segment = {
        .low_limit = 0xFFFF,
        .low_base = 0,
        .mid_base = 0,
        .access_byte = 0xF2,
        .limit_and_flags = 0x20,
        .high_base = 0,
    },
};

static gdtr_t gdtr = (gdtr_t){
    .limit = sizeof(gdt_t) - 1,
    .base = (uint64_t)&gdt};

void gdt_init(void)
{
    __asm__ volatile("lgdt %0\n"
                 :
                 : "m"(gdtr));

    reload_gdt();

    info("Initialized");
}
