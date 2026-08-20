#pragma once

#include <misc/ansii.h>
#include <devs/serial.h>


#ifdef __DEBUG__
#define srdebug(from, fmt, ...)  srprintf("[   " ANSII_RED_FG "DEBUG" ANSII_RESET "   ]" ANSII_GRAY_FG " %s" ANSII_RESET ": " fmt "\n", #from, ##__VA_ARGS__)
#define dsrdebug(from, fmt, ...) srprintf("[%s(%s:%d)]: " fmt "\n", #from, __FILE__, __LINE__, ##__VA_ARGS__)
#define debug(fmt, ...)          srprintf("[   " ANSII_RED_FG "DEBUG" ANSII_RESET "   ] " fmt "\n", ##__VA_ARGS__)
#define frdebug(from, fmt, ...)  srprintf("[%s]: " fmt "\n", from, ##__VA_ARGS__)
#define at(num)                  debug("Reached at: %d", num)
#else
#define srdebug(from, ...)
#define dsrdebug(from, fmt, ...)
#define debug(fmt, ...)
#define frdebug(from, fmt, ...)
#define at(num)
#endif

