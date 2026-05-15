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
    printf("size: %lu\n", size);

    sbrk(0);
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
    // printf("we are inside find_free()\n");
    while (current != NULL)
    {
        if(current->free && current->size >= size + sizeof(block_t) + MIN_RESERVED_BLOCK_SPACE)
            return current;
        *last = current;
        current = current->next;
    }

    // couldn't find
    return NULL;
}

static int
split_block(block_t *block, size_t size)
{
    // if (block->size < size + sizeof(block_t) + MIN_RESERVED_BLOCK_SPACE)
    // {
    //     fprintf(stderr, "can't split block, not enough space left!\n");
    //     return -1;
    // }
    //
    block_t *next_block = block->next;
    block_t *new_next_block = (block_t *)((char *)block + sizeof(block_t) + size);
    new_next_block->size = block->size - size - sizeof(block_t);
    printf("new_size: %lu = %lu - %lu - %lu\n", new_next_block->size, block->size, size, sizeof(block_t));
    // block->free = 0;
    block->size = size;
    
    block->next = new_next_block;
    new_next_block->prev = block;
    if(next_block)
        next_block->prev = new_next_block;
    new_next_block->next = next_block;
    new_next_block->free = 1;
        
    return 0;
}

void
*my_malloc(size_t bytes)
{
    if (bytes <= 0)
    {
        fprintf(stderr, "No size provided!\n");
        return NULL;
    }

    bytes = align_size(bytes);

    block_t *last = heap_start;
    block_t *block = find_free(&last, bytes);
    if (!block)
    {
        // printf("we are inside (!block) from my_malloc()\n");
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
    
    // printf("returned from my_malloc()\n");
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

static void
coalesce(block_t **block)
{
    block_t *leftmost = *block;
    block_t *rightmost = *block;

    size_t size = (*block)->size;
    // size_t old_size = (*block)->size;
    while (leftmost->prev && leftmost->prev->free) 
    {
        leftmost = leftmost->prev;
        size += leftmost->size + sizeof(block_t);
        // printf("coalesce->size: %lu\n", size);
    }
    while (rightmost->next && rightmost->next->free) 
    {
        rightmost = rightmost->next;
        size += rightmost->size + sizeof(block_t);
        // printf("coalesce->size: %lu\n", size);
    }

    if (leftmost == *block && rightmost == *block) return;

    leftmost->size = size;
    leftmost->next = rightmost->next;
    // printf("coalesce->size: %lu\n", size);
    // void *ptr = (void *)leftmost + sizeof(block_t);
    // memset(ptr, 0xDE, size);
    *block = leftmost;
}

void
my_free(void *ptr)
{
    if(!ptr)
    {
        fprintf(stderr, "invalid ptr!\n");
        return;
    }

    block_t *temp = (block_t *)((char *)ptr - sizeof(block_t));
    temp->free = 1;
    coalesce(&temp);
    // printf("inside free(): size: %lu\n", temp->size);
    ptr = (void *)temp + sizeof(block_t);
    memset(ptr, 0xDE, temp->size);
    printf("inside free(): size: %lu\n", temp->size);
}

void
heap_print()
{
    block_t *current = heap_start;
    while (current)
    {
        sleep(1);
        char *free = current->free ? "YES" : "NO";
        printf("block { size: %lu, free: %s }\n", 
                current->size, free);
        current = current->next;
    }
}
