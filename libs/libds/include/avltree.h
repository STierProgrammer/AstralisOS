#pragma once

#include <stddef.h>

typedef struct avlnode_t avlnode_t;
typedef struct avlnode_t
{
    avlnode_t *left, *right;
    int height;
} avlnode_t;

typedef int(avltree_cmp_fn_t)(avlnode_t *a, avlnode_t *b);

avlnode_t *avltree_insert(avlnode_t *at, avlnode_t *node, avltree_cmp_fn_t cmp_fn);
avlnode_t *avltree_delete(avlnode_t *at, avlnode_t *node, avltree_cmp_fn_t *cmp_fn);

