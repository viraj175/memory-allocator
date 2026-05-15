#include "malloc.h"
#include <stdio.h>
#include <string.h>

int
main()
{
    int *array = my_malloc(sizeof(int) * 10);
    char *name = my_malloc(6);
    strncpy(name, "viraj", 6);
    printf("block: %lu\n", sizeof(block_t));
    for (int i = 0; i < 10; ++i)
    {
        array[i] = i;
        printf("%d\t", array[i]);
    }
    printf("\n");
    printf("name: %s\n", name);
    my_free(array);
    my_free(name);
    printf("name: %s\n", name);
    heap_print();
    return 0;
}
