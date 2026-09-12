#pragma once

#include <stdarg.h>

void _printf(void *priv, void (*put)(void*, int), void (*puts)(void*, const char *), const char *fmt, va_list args);

