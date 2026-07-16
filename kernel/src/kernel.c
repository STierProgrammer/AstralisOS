#include <misc/panic.h>
#include <misc/darray.h>
#include <mm/alloc.h>
#include "libinput/include/kbd.h"
#include <drvs/cmos.h>
#include <drvs/rtc/rtc.h>
#include "tasks/sched.h"
#include <libinput/include/mse.h>
#include <devs/dev.h>
#include <fs/tar.h>
#include <fs/tmpfs.h>
#include <fs/vfs.h>
#include <mm/vheap.h>
#include <modules.h>
#include <kernel.h>
#include <fs/initrd.h>

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdatomic.h>

#include <bootstub.h>

#include <misc/debug.h>

#include <mm/pmm/pmm.h>
#include <mm/vmm/vmm.h>
#include <mm/slab.h>

#include <devs/serial.h>

#include <drvs/ps2/ps2.h>
#include <drvs/ps2/kbd/ps2_kbd.h>
#include <drvs/ps2/ps2_mouse.h>

#include <gfx/surface.h>

#include <firmware/acpi/acpi.h>

#include <misc/arrlen.h>

#ifdef __ARCH_X86_64__
#include <arch/x86_64/mm/paging.h>
#include <arch/x86_64/cpu/cpu.h>
#include <arch/x86_64/desc/gdt.h>
#include <arch/x86_64/desc/idt.h>
#include <arch/x86_64/ints/pic.h>
#include <arch/x86_64/ints/apic.h>
#include "arch/x86_64/desc/tss.h"
#endif

#include <terminal/fbtty.h>

krnl_ctx_t krnl_ctx;

void kmain(bootloader_ctx_t *ctx)
{
    krnlctx(bootloader_ctx) = ctx;

    serial_init();
    gdt_init();
    idt_init();
    tss_init(); 
    pmm_init();

    krnlctx(pt) = pt_create();    
    pt_map_kernel(krnlctx(pt));
    pt_map_memmap(krnlctx(pt));
    pt_swap(krnlctx(pt));

    acpi_init();
    pic_init();
    apic_init();

    vheap_init();
    slab_init();
    kmalloc_init();
    devs_init();

    ps2_init();
    ps2_keyboard_init();
    ps2_mouse_init();

    vfs_init();
    fs_mount(&tmpfs, &vfs_root);

    initrd_init();
    
    fb_t fb;
    bootctx(fbs).get_fb(&bootctx(fbs), &fb, 0);

    inode_t *sigma_txt = NULL;
    const path_t sigma_txt_path = vfs_path_from_abs("/sigma.txt");
    vfs_create(&sigma_txt_path, INODE_FILE, &sigma_txt);
    char buf[256] = { 0 };
    const char *text = "Hello, World!";
    size_t len = strlen(text);
    memcpy(buf, text, len);
    inode_write(sigma_txt, buf, len + 1, 0);
    
    inode_t *cool = NULL;
    initrd_get("cool", &cool); 

    static char asmbuf[4096] = { 0 };
    inode_read(cool, asmbuf, 4096, 0);
    
    task_t *user_task = user_task_create((uint64_t)asmbuf, 4096);
    sched_schedule(user_task); 
    
    sched_init();
    while (1)
        task_yield();
    hcf();
}
