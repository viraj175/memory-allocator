#include "malloc.h"
#include <stdio.h>

int
main()
{
    int *array = my_malloc(10);
    for (int i = 0; i < 10; ++i)
    {
        array[i] = i;
        printf("%d\t", array[i]);
    }
    printf("\n");
    return 0;
}
