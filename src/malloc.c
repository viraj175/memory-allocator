#include "malloc.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>

static block_t *head_start = NULL;
