#pragma once
#include <libds/include/list.h>
#include <stdarg.h>
#include <misc/ansii.h>

#define LOGGER_NAMELEN_MAX 64

typedef struct logger_t logger_t;
typedef struct logger_t {
    list_t list;
    char name[LOGGER_NAMELEN_MAX];
    void *priv;
    void (*log)(void *priv, const char *fmt, va_list args);
} logger_t;

void logger_register(logger_t *logger);
void logger_broadcast(const char *fmt, ...);

#define info(fmt, ...) (logger_broadcast("[   " "INFO" "    ]" " %s" ": " fmt "\n", __func__, ##__VA_ARGS__))


