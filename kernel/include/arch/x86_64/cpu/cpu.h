#pragma once

static inline void hcf(void)
{
    for (;;)
    {
        asm ("hlt");
    }
}
