#pragma once

#include <stdint.h>

#define PAGE_SIZE 4096
#define ALIGN_UP(address, alignment) (((address) + (alignment - 1)) & ~((alignment) - 1))
#define ALIGN_DOWN(address, alignment) ((address) & ~((alignment) - 1))

typedef uint64_t virt_addr_t;
typedef uint64_t physc_addr_t;
