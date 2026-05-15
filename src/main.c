#include "malloc.h"
#include <stdio.h>
#include <string.h>

struct names {
    char name[10];
    size_t length;
};

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

    printf("struct size: %lu\n", sizeof(struct names));
    struct names *characters = my_malloc(sizeof(struct names) * 2);  
    int *nums = my_calloc(sizeof(int), 8);
    nums = my_realloc(nums, sizeof(int) * 20);
    my_free(characters);
    my_free(nums);

    heap_print();
    return 0;
}
