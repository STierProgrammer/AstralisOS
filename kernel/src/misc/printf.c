#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <misc/debug.h>

void _printf(void *priv, void (*put)(void*, int), void (*puts)(void*, const char *), const char *fmt, va_list args)
{
    while (*fmt)
    {
        if (*fmt == '%')
        {
            fmt++;
            switch (*fmt)
            {
            case 's':
            {
                const char *value = va_arg(args, const char *);
                puts(priv, value);
                break;
            }
            case 'c':
            {
                int value = va_arg(args, int);
                put(priv, (char)value);
                break;
            } 
            case 'd':
            {
                int value = va_arg(args, int);

                if (value == 0)
                {
                    put(priv, '0');
                    break;
                }

                if (value < 0)
                {
                    put(priv, '-');
                    value = -value;
                }

                char str[64] = {0};
                int i = 0;
                while (value > 0)
                {
                    char digit = value % 10 + '0';
                    value /= 10;
                    str[i++] = digit;
                }

                for (int i = 63; i >= 0; i--)
                {
                    put(priv, str[i]);
                }

                break;
            }
            case 'z':
            {
                if (*(fmt + 1) && *(fmt + 1) == 'u')
                {
                    fmt++;
                    size_t value = va_arg(args, size_t);

                    if (value == 0)
                    {
                        put(priv, '0');
                        break;
                    }

                    char str[64] = {0};
                    int i = 0;
                    while (value > 0)
                    {
                        char digit = value % 10 + '0';
                        value /= 10;
                        str[i++] = digit;
                    }

                    for (int i = 63; i >= 0; i--)
                    {
                        put(priv, str[i]);
                    }
                }
                else
                {
                    fmt--;
                }

                break;
            }
            case 'x':
            {
                uint64_t value = va_arg(args, uint64_t);

                for (int i = 2 * sizeof(uint64_t); i > 0; i--)
                {
                    int new_val = (value >> ((i - 1) * 4)) & 0xF;

                    if (new_val < 10)
                    {
                        put(priv, new_val + '0');
                    }
                    else
                    {
                        put(priv, new_val + 'A' - 10);
                    }
                }

                break;
            }
            case 'b':
            {
                uint64_t value = va_arg(args, uint64_t);
                for (size_t i = 8 * sizeof(uint64_t); i > 0; i--)
                {
                    uint8_t new_val = (value >> (i - 1)) & 0b1;
                    put(priv, new_val + '0');
                }
                break;
            }
            case '%':
            {
                put(priv, '%');

                break;
            }
            }
        }
        else
        {
            put(priv, *fmt);
        }

        fmt++;
    }
}
