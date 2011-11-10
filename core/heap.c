#include <stdlib.h>
#include "heap.h"

void *allocate_value(size_t size)
{
    return malloc(size);
}

void *allocate_cell(size_t size)
{
    return malloc(size);
}
