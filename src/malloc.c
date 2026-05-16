#include "malloc.h"
#include <stdint.h>
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
    while (current != NULL)
    {
        if(current->free && current->size >= size)
            return current;
        *last = current;
        current = current->next;
    }

    return NULL;
}

static int
split_block(block_t *block, size_t size)
{
    if (block->size < size + MIN_RESERVED_BLOCK_SPACE)
    {
        fprintf(stderr, "can't split block, not enough space left!\n");
        return -1;
    }

    block_t *next_block = block->next;
    block_t *new_next_block = (block_t *)((char *)block + sizeof(block_t) + size);
    new_next_block->size = block->size - size - sizeof(block_t);
    printf("new_size: %lu = %lu - %lu - %lu\n", 
            new_next_block->size, block->size, size, sizeof(block_t));
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
    if (bytes == 0)
    {
        fprintf(stderr, "No size provided!\n");
        return NULL;
    }

    bytes = align_size(bytes);

    block_t *last = heap_start;
    block_t *block = find_free(&last, bytes);
    if (!block)
    {
        block = request_memory(sizeof(block_t) + bytes);
        if (!block) return NULL;
        block->size = bytes;
        block->free = 0;
        block->next = NULL;
        block->prev = last;

        if (last)
            last->next = block;

        if (!heap_start)
        {
            heap_start = block;
            block->prev = NULL;
        }
    }
    else 
    {
        block->free = 0;
        split_block(block, bytes);
    }
    
    return (void *)((char *)block + sizeof(block_t));
}

void
*my_calloc(size_t n, size_t bytes)
{
    if (!n || !bytes)
    {
        fprintf(stderr, "invalid size!\n");
        return NULL;
    }

    if (bytes > SIZE_MAX / n)
    {
        fprintf(stderr, "overflow!\n");
        return NULL;
    }

    void *ptr = my_malloc(n * bytes);
    memset(ptr, 0, n * bytes);
    return ptr;
}

void
*my_realloc(void *ptr, size_t bytes)
{
    if (!ptr)
    {
        fprintf(stderr, "can't reallocate invalid memory!\n");
        return NULL;
    }
    if (bytes == 0)
    {
        my_free(ptr);
        return NULL;
    }

    bytes = align_size(bytes);
    block_t *block = (block_t *)((char *)ptr - sizeof(block_t));

    if (bytes == block->size)
        return ptr;

    if (block->size > MIN_RESERVED_BLOCK_SPACE && bytes < block->size - MIN_RESERVED_BLOCK_SPACE)
    {
        split_block(block, bytes);
        return ptr;
    }

    if (bytes < block->size)
        return ptr;

    if (block->next && block->next->free &&
        block->size + sizeof(block_t) + block->next->size >= bytes)
    {
        block->size += sizeof(block_t) + block->next->size;
        block->next = block->next->next;
        if (block->next)
            block->next->prev = block;
        if (block->size > MIN_RESERVED_BLOCK_SPACE && bytes < block->size - MIN_RESERVED_BLOCK_SPACE)
            split_block(block, bytes);
        return ptr;
    }

    void *new_ptr = my_malloc(bytes);
    if (!new_ptr) return NULL;
    memcpy(new_ptr, ptr, block->size);
    my_free(ptr);
    return new_ptr;
}

static void
coalesce(block_t **block)
{
    block_t *leftmost = *block;
    block_t *rightmost = *block;

    size_t size = (*block)->size;
    while (leftmost->prev && leftmost->prev->free) 
    {
        leftmost = leftmost->prev;
        size += leftmost->size + sizeof(block_t);
    }
    while (rightmost->next && rightmost->next->free) 
    {
        rightmost = rightmost->next;
        size += rightmost->size + sizeof(block_t);
    }

    if (leftmost == *block && rightmost == *block) return;

    leftmost->size = size;
    leftmost->next = rightmost->next;
    if (rightmost->next)
        rightmost->next->prev = leftmost;

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

    ptr = (char *)temp + sizeof(block_t);
    memset(ptr, 0xDE, temp->size);
}

void
heap_print()
{
    block_t *current = heap_start;
    while (current)
    {
        // sleep(1);
        char *free = current->free ? "YES" : "NO";
        printf("block { size: %lu, free: %s }\n", 
                current->size, free);
        current = current->next;
    }
}
