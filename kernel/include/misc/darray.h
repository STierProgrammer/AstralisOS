// NOTE: Inspired by nob's da_append macros
#pragma once

#include <mm/alloc.h>

#define da_reserve(da, extra) \
   do {\
      if((da)->len + extra >= (da)->cap) {\
          kfree((da)->items, (da)->cap*sizeof(*(da)->items));\
         (da)->cap = (da)->cap*2+extra;\
          void *new_items = kmalloc((da)->cap*sizeof(*(da)->items));\
         (da)->items = new_items;\
      }\
   } while(0)

#define da_push(da, value) \
   do {\
        da_reserve(da, 1);\
        (da)->items[(da)->len++]=value;\
   } while(0)
   
#define da_free(da) free((da).items)
