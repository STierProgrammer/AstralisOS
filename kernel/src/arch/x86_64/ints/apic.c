#include "arch/x86_64/ints/pic.h"
#include "arch/x86_64/pit.h"
#include <kernel.h>
#include <arch/x86_64/mm/paging.h>
#include <misc/debug.h>

#include <bootstub.h>

#include <stdbool.h>
#include <stdint.h>

#define CPUID_FEATURE_HAS_LOCAL_APIC (1 << 9)
#define CPUID_FEATURE_HAS_MSR        (1 << 5)

#define APIC_BASE_MSR 0x1B
#define APIC_BASE_ENABLE (1ULL << 11)

static const int_ctrl_t apic;

extern volatile uint64_t lapic_addr;
extern volatile uint64_t ioapic_addr;
extern volatile uint64_t ioapic_gsi_base;

static uint32_t volatile *lapic = NULL;
static uint32_t volatile *ioapic = NULL;

bool cpuid_apic_check(void)
{
    uint32_t eax = 1, edx;
    __asm__ volatile (
            "cpuid"
            : "=d"(edx)
            : "a"(eax)
    );
    return edx & CPUID_FEATURE_HAS_LOCAL_APIC;
}

bool cpuid_has_msr(void)
{
    uint32_t eax = 1, edx;
     __asm__ volatile (
            "cpuid"
            : "=d"(edx)
            : "a"(eax)
    );
    return edx & CPUID_FEATURE_HAS_MSR;
}

uint64_t msr_get(uint32_t msr)
{
    uint32_t lo = 0, hi = 0;

    __asm__ volatile (
        "rdmsr"
        : "=a"(lo), "=d"(hi)
        : "c"(msr)
    );

    return ((uint64_t)hi << 32) | (uint64_t)lo;
}

void msr_set(uint32_t msr, uint64_t val)
{
    uint32_t lo = val & 0xFFFFFFFF;
    uint32_t hi = val >> 32;
    __asm__ volatile(
           "wrmsr" 
           : 
           : "a"(lo), "d"(hi), "c"(msr)
    );
}

#define LAPIC_REGISTER_TIMER_DIV             0x3E0
#define LAPIC_REGISTER_TIMER_INITCNT         0x380
#define LAPIC_REGISTER_TIMER                 0x320
#define LAPIC_REGISTER_TIMER_CURRENT_COUNT   0x390
#define LAPIC_REGISTER_EOI                   0x0B0

static inline void lapic_write(uint32_t reg, uint32_t val)
{
    lapic[reg / 4] = val;
}

static inline uint32_t lapic_read(uint32_t reg)
{
    return lapic[reg / 4];
}

static inline void ioapic_write(uint32_t reg, uint32_t val)
{
    ioapic[0] = reg & 0xff;
    ioapic[4] = val;
}

static inline uint32_t ioapic_read(uint32_t reg)
{
    ioapic[0] = reg & 0xff;
    return ioapic[4];
}

#define IOREDTBL_DELIVERY_MODE_SHIFT    8
#define IOREDTBL_DESTINATION_MODE_SHIFT 11
#define IOREDTBL_PIN_POLARITY_SHIFT     13
#define IOREDTBL_TRIGGER_MODE_SHIFT     15
#define IOREDTBL_DESTINATION_SHIFT      24
#define IOREDTBL_MASK_SHIFT             16

void ioapic_irq_clear_mask(uint8_t irq)
{
    size_t number = ioapic_gsi_base + (irq * 2);
    uint32_t reg_low = ioapic_read(0x10 + number);

    reg_low = (reg_low & ~(1 << IOREDTBL_MASK_SHIFT));
    ioapic_write(0x10 + number, reg_low);
}

void ioapic_register(uint8_t irq, uint8_t vec, uint8_t lapic_id)
{
    size_t number = ioapic_gsi_base + (irq * 2);
    uint32_t reg_low = ioapic_read(0x10 + number),
             reg_high = ioapic_read(0x10 + number + 1);

    reg_low  = (reg_low & ~0xFF) | vec;
    reg_low  = (reg_low & ~((0b111 << IOREDTBL_DELIVERY_MODE_SHIFT) | (1 << IOREDTBL_DESTINATION_MODE_SHIFT)));
    reg_low &= ~(1 << IOREDTBL_PIN_POLARITY_SHIFT | 1 << IOREDTBL_TRIGGER_MODE_SHIFT);

    reg_high = (reg_high & ~(0xFF << IOREDTBL_DESTINATION_SHIFT)) | (((uint32_t)lapic_id) << IOREDTBL_DESTINATION_SHIFT);

    ioapic_write(0x10 + number, reg_low);
    ioapic_write(0x10 + number + 1, reg_high);
}

void apic_timer_init(void)
{
    lapic_write(LAPIC_REGISTER_TIMER_DIV, 0x3);
    lapic_write(LAPIC_REGISTER_TIMER_INITCNT, 0xFFFFFFFF);

    pit_init(1000);
    
    pit_sleep_ms(10);
    
    uint64_t ticks = (0xFFFFFFFF - lapic_read(LAPIC_REGISTER_TIMER_CURRENT_COUNT)) / 10;

    lapic_write(LAPIC_REGISTER_TIMER, 0x20000 | 0x20);
    lapic_write(LAPIC_REGISTER_TIMER_INITCNT, ticks);
}

void apic_send_eoi(uint8_t irq)
{
    (void)irq;
    lapic_write(LAPIC_REGISTER_EOI, 0);
}

uint32_t get_lapic_id(void)
{
    return lapic_read(0x20) >> 24;
}

void apic_init(void)
{   
    if (!cpuid_apic_check())
    {
        srdebug(apic_init, "There is no support for APIC!");
        return;
    }
    
    uint64_t apic_msr = msr_get(APIC_BASE_MSR);
    if (!(apic_msr & APIC_BASE_ENABLE))
    {
        srdebug(apic_init, "APIC is disabled!");
        return;
    }

    apic_msr &= ~PAGE_PHYSICAL_ADDRESS_MASK;
    apic_msr |= (lapic_addr & PAGE_PHYSICAL_ADDRESS_MASK);
    msr_set(APIC_BASE_MSR, apic_msr);    

    lapic = (uint32_t*)to_vaddr(lapic_addr);
    debug("lapic: %x", lapic_addr);

    pt_map(krnlctx(pt), lapic_addr, (vaddr_t)lapic, PAGE_FLAG_READ_WRITE | PAGE_FLAG_PRESENT);

    ioapic = (uint32_t*)to_vaddr(ioapic_addr);
    pt_map(krnlctx(pt), ioapic_addr, (vaddr_t)ioapic, PAGE_FLAG_READ_WRITE | PAGE_FLAG_PRESENT);

    debug("ioapic: %x", ioapic);

    apic_timer_init();
    pic_disable();

    ioapic_register(1, 0x21, get_lapic_id());
    ioapic_register(12, 0x2C, get_lapic_id());
    ioapic_register(4, 0x30, get_lapic_id());

    krnlctx(interrupt_controller) = &apic;
    info("Initalized!");
}

void apic_set_mask(uint8_t irq)
{
    (void)irq;
}

static const int_ctrl_t apic = {
    .name = "APIC",
    .send_eoi = apic_send_eoi,
    .set_mask = apic_set_mask,
    .clear_mask = ioapic_irq_clear_mask,
};


