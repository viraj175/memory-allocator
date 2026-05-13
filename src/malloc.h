#ifndef MALLOC_H
#define MALLOC_H

#include <stddef.h>
#define POOL_SIZE 8 * 1024
#define FREE_LIST_SIZE 50

typedef struct block {
    size_t size;
    struct block *next;
    struct block *prev;
    int free;
} block_t;

void *my_malloc(size_t bytes);
void *my_calloc(size_t n, size_t bytes);
void my_free(void *ptr);
void *my_realloc(void *ptr, size_t bytes);

void heap_print(void);

#endif // !MALLOC_H


