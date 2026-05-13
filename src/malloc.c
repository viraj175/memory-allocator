#include "malloc.h"
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

static block_t *heap_start = NULL;

static size_t
align_size(size_t size)
{
    return (size + 7) & ~7;
}

static block_t
*request_memory(size_t size)
{
    if(size == 0)
    {
        fprintf(stderr, "0 bytes requested!");
        return NULL;
    }

    block_t *block = sbrk(0);
    void *request = sbrk(size);

    if (request == (void *)-1)
    {
        perror("memory allocation failed!");
        return NULL;
    }

    return request;
}

static block_t
*find_free(block_t **last, size_t size)
{
    block_t *current = heap_start;
    printf("we are inside find_free()\n");
    while (current != NULL)
    {
        if(current->free && current->size >= size)
            return current;
        *last = current;
        current = current->next;
    }

    return NULL;
}

static void
split_block(block_t *block, size_t size)
{
    if (block->size > size + sizeof(block_t) + MIN_RESERVED_BLOCK_SPACE)
    {
        fprintf(stderr, "can't split block, not enough space left!\n");
        return;
    }

    block_t *next_block = block->next;
    block_t *new_next_block = (void *)block + sizeof(block_t) + block->size;
    new_next_block->size = block->size - size - sizeof(block_t);
    block->free = 0;
    block->size = size;
    
    block->next = new_next_block;
    new_next_block->prev = block;
    new_next_block->next = next_block;
    next_block->prev = new_next_block;
}

void
*my_malloc(size_t bytes)
{
    if (bytes <= 0)
    {
        fprintf(stderr, "No size provided!\n");
        return NULL;
    }

    block_t *last = heap_start;
    block_t *block = find_free(&last, bytes);
    if (!block)
    {
        printf("we are inside (!block) from my_malloc()\n");
        block = request_memory(sizeof(block_t) + bytes);
        if (!block) return NULL;
        block->size = bytes;
        block->free = 0;
        block->next = NULL;
        block->prev = last;

        if (last)
            last->next = block;

        if (!heap_start)
            heap_start = block;
    }
    else 
    {
        block->free = 0;
        split_block(block, bytes);
    }
    
    printf("returned from my_malloc()\n");
    return (void *)((char *)block + sizeof(block_t));
}

void
*my_calloc(size_t n, size_t bytes)
{
    return NULL;
}

void
*my_realloc(void *ptr, size_t bytes)
{
    return NULL;
}
