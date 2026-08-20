#include <stddef.h>

#include <arch/x86_64/desc/tss.h>
#include <arch/x86_64/desc/gdt.h>

#include <misc/debug.h>

tss_t tss = {0};

void tss_init(void)
{
    tss_entry_t *tss_segment = &gdt.tss_segment;
    tss_segment->low_limit = sizeof(tss_t) - 1;
    uint64_t tss_ptr = (uint64_t)&tss;
    tss_segment->low_base = tss_ptr;
    tss_segment->mid_base = tss_ptr >> 16;
    tss_segment->high_base = tss_ptr >> 24;
    tss_segment->highest_base = tss_ptr >> 32;
    tss_segment->access_byte = 0x89;
    tss_segment->reserved = 0;
    tss_segment->limit_and_flags = 0x20;

    __asm__ volatile(
        "ltr %0"
        :
        : "r"((uint16_t)offsetof(gdt_t, tss_segment)));

        
    info("Initialized");
}
