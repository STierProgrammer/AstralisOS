#pragma once
#include <stddef.h>
#include <stdbool.h>

// NOTE: This implementation is a slightly modified version of that of the linux kernel itself.
// https://github.com/torvalds/linux/blob/b1bc554e009e3aeed7e4cfd2e717c7a34a98c683/tools/firewire/list.h
//
// Things like list_insert, list_append are all literally the exact same, with the exception 
// of list_remove which also closes in the array element removed since the linux version didn't clean it up

typedef struct list_t list_t;
typedef struct list_t 
{
    list_t *next, *prev;
} list_t;

static inline void list_init(list_t *list)
{
    list->next = list;
    list->prev = list;
}

static inline bool list_empty(list_t *list)
{
    return list == list->next;
}

// a -> c
// a -> b -> c
static inline void list_insert(list_t *at, list_t *new)
{
    new->next = at;
    new->prev = at->prev;
    new->prev->next = new;
    new->next->prev = new;
}

static inline void list_append(list_t *into, list_t *new)
{
    list_insert(into, new);
}

/// a <-> b <-> c
/// a <-> c
static inline void list_remove(list_t *list) {
    list->prev->next = list->next;
    list->next->prev = list->prev;
    list->next = list;
    list->prev = list;
}

static inline list_t* list_next(list_t* list) {
    return list->next != list ? list->next : NULL;
}

#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define foreach(var, head) for (list_t *var = (head)->next; var != (head); var = var->next)


