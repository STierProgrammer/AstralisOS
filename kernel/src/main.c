#include "bootstub.h"

#ifdef ARCH_X86_64
#include "arch/x86_64/cpu/cpu.h"
#include "arch/x86_64/devs/serial.h"
#elif ARCH_AARCH64
#include "arch/aarch64/cpu/cpu.h"
#endif

void kmain(bootctx_t *ctx)
{
    (void)ctx;
    srputs("Hello, World!");
    hcf();
}


