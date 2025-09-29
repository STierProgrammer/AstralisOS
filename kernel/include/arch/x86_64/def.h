#pragma once

#include <stdint.h>
#include <stddef.h>

#define PAGE_SIZE 4096
#define ALIGN_UP(address, alignment) (((address) + (alignment - 1)) & ~((alignment) - 1))
#define ALIGN_DOWN(address, alignment) ((address) & ~((alignment) - 1))

#define enable_interrupts() __asm__ ("cli")
#define disable_interrupts() __asm__ ("sti")

typedef uint64_t virt_addr_t;
typedef uint64_t physc_addr_t;

static inline void hcf(void)
{
  for (;;)
  {
    __asm__("hlt");
  }
}

static inline void outb(uint16_t port, uint8_t val)
{
  __asm__ volatile("outb %b0, %w1" ::"a"(val), "Nd"(port) : "memory");
}

static inline uint8_t inb(uint16_t port)
{
  uint8_t ret;
  __asm__ volatile("inb %w1, %b0" : "=a"(ret) : "Nd"(port) : "memory");
  return ret;
}

static inline uint64_t read_cr3(void)
{
    uint64_t val;
    __asm__ volatile(
        "mov %%cr3, %0" : "=r"(val));
    return val;
}

static inline void set_cr3(uint64_t val)
{
    __asm__ volatile("mov %0, %%cr3" :: "r"(val));
}

// NOTE: This implementation is a slightly modified version of that of the linux kernel itself.
// https://github.com/torvalds/linux/blob/b1bc554e009e3aeed7e4cfd2e717c7a34a98c683/tools/firewire/list.h
//
// Things like list_insert, list_append are all literally the exact same, with the exception 
// of list_remove which also closes in the array element removed since the linux version didn't clean it up
typedef struct list_t 
{
    struct list_t *next, *prev;
} list_t;

static inline void list_init(list_t *list) {
    list->next = list;
    list->prev = list;
}

static inline list_t *list_last(list_t *list) {
    return list->prev != list ? list->prev : NULL;
}

static inline list_t *list_next(list_t *list) {
    return list->next != list ? list->next : NULL;
}

static inline void list_insert(list_t *new, list_t *link) {
    new->prev = link->prev;
    new->next = link;
    new->prev->next = new;
    new->next->prev = new;
}

static inline void list_append(list_t *new, list_t *into) {
    list_insert(new, into);
}

static inline void list_remove(list_t *list) {
    list->prev->next = list->next;
    list->next->prev = list->prev;
    list->next = list;
    list->prev = list;
}