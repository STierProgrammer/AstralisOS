#pragma once

#include <stdint.h>

#define INTERRUPT_GATE 0x8E

typedef struct idtr_t
{
    uint16_t size;
    uint64_t offset;
} __attribute__((packed)) idtr_t;

typedef struct idt_descriptor_t
{
    uint16_t lower_offset;
    uint16_t segment_selector;
    uint8_t ist;
    uint8_t type_and_attributes;
    uint16_t mid_offset;
    uint32_t high_offset;
    uint32_t reserved;
} __attribute__((packed)) idt_descriptor_t;

typedef struct idt_t
{
    idt_descriptor_t descriptors[256];
} __attribute__((packed)) idt_t;

void create_descriptor(uint8_t index, uint8_t type_and_attributes, void (*handler)());
void init_idt(void);

