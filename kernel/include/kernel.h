#pragma once

#include <bootstub.h>
#include <stdint.h>

typedef intptr_t ssize_t;

#define PAGE_SIZE 4096

#define to_vaddr(paddr) (vaddr_t)(((uintptr_t)(paddr)) + krnl_ctx.bootloader_ctx->hhdm)
#define to_paddr(vaddr) ((paddr_t)(((uintptr_t)(vaddr)) - krnl_ctx.bootloader_ctx->hhdm))

#define paddr_ptr(paddr) (void*)(to_vaddr((paddr)))

#define krnlctx(v) krnl_ctx.v
#define bootctx(v) krnlctx(bootloader_ctx)->v

typedef struct page_table_t page_table_t;
typedef struct vmm_t vmm_t;

typedef struct int_ctrl_t 
{
    const char *name;
    void (*uninit)(void);
    void (*send_eoi)(uint8_t irq);
    void (*set_mask)(uint8_t irq);
    void (*clear_mask)(uint8_t irq);
} int_ctrl_t;

typedef struct krnl_ctx_t
{
    bootloader_ctx_t* bootloader_ctx;
    page_table_t *pt;
    const int_ctrl_t* interrupt_controller;
    vmm_t *krnl_vmm;
} krnl_ctx_t;

extern krnl_ctx_t krnl_ctx;

typedef uintptr_t vaddr_t;
typedef uintptr_t paddr_t;


