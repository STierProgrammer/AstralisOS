#pragma once

extern void sse_enable(void);

float sqrt_sse(float x)
{
    float result;
    __asm__ volatile(
        "sqrtss %1, %0"
        : "=x"(result)
        : "x"(x)
    );
    return result;
}

