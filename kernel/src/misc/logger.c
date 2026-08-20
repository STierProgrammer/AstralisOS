#include <misc/logger.h>
#include <stdarg.h>

static list_t head = LIST_HEAD_INIT(head);

void logger_register(logger_t *logger)
{
    list_append(&head, (list_t*)logger);
}

void logger_broadcast(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    foreach(node, &head)
    {
        va_list copy;
        va_copy(copy, args);

        logger_t *logger = (logger_t*)node;
        logger->log(logger->priv, fmt, copy);
        va_end(copy);
    }

    va_end(args);
}

