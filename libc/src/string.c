#include <string.h>
#include <stdint.h>
    
// GCC and Clang reserve the right to generate calls to the following
// 4 functions even if they are not directly called.
// Implement them as the C specification mandates.
// DO NOT remove or rename these functions, or stuff will eventually break!
// They CAN be moved to a different .c file.

void *memcpy(void *dest, const void *src, size_t n)
{
    uint8_t *pdest = (uint8_t *)dest;
    const uint8_t *psrc = (const uint8_t *)src;

    for (size_t i = 0; i < n; i++)
    {
        pdest[i] = psrc[i];
    }

    return dest;
}

void *memset(void *s, int c, size_t n)
{
    uint8_t *p = (uint8_t *)s;

    for (size_t i = 0; i < n; i++)
    {
        p[i] = (uint8_t)c;
    }

    return s;
}

void *memset_u32(void *s, uint32_t c, size_t n)
{
    uint32_t *p = (uint32_t *)s;

    for (size_t i = 0; i < n; i++)
    {
        p[i] = c;
    }

    return s;
}

void *memmove(void *dest, const void *src, size_t n)
{
    uint8_t *pdest = (uint8_t *)dest;
    const uint8_t *psrc = (const uint8_t *)src;

    if (src > dest)
    {
        for (size_t i = 0; i < n; i++)
        {
            pdest[i] = psrc[i];
        }
    }
    else if (src < dest)
    {
        for (size_t i = n; i > 0; i--)
        {
            pdest[i - 1] = psrc[i - 1];
        }
    }

    return dest;
}

int memcmp(const void *s1, const void *s2, size_t n)
{
    const uint8_t *p1 = (const uint8_t *)s1;
    const uint8_t *p2 = (const uint8_t *)s2;

    for (size_t i = 0; i < n; i++)
    {
        if (p1[i] != p2[i])
        {
            return p1[i] < p2[i] ? -1 : 1;
        }
    }

    return 0;
}

void strcpy(char *dest, const char *src)
{
    while (*src != '\0') *(dest++) = *(src++);
}

void strncpy(char *dest, const char *src, size_t size)
{
    for (size_t i = 0; i < size && *src; i++)
    {
        *dest = *src;
    }
}

int strcmp(const char *s1, const char *s2)
{
    unsigned char c1 = *s1;
    unsigned char c2 = *s2;

    while (*s1 && *s2)
    {
        c1 = *s1;
        c2 = *s2;
        if (c1 != c2)
            break;
        s1++;
        s2++;
    }

    return c1 - c2;
}

int strncmp(const char *s1, const char *s2, size_t n)
{
    unsigned char c1 = *s1;
    unsigned char c2 = *s2;
    for (size_t i = 0; i < n; ++i)
    {
        c1 = *s1;
        c2 = *s2;
        if (c1 != c2) break;
        s1++;
        s2++;
    }

    return c1 - c2;
}

size_t strlen(const char *str)
{
    size_t size = 0;
    while (*str++) size++;
    return size;
}


