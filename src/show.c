#include <stdio.h>
#include "show.h"

int bd_show_indent(int depth)
{
    int i, acc = 0;
    for(i = 0; i < depth; i++){
        acc += printf("  ");
    }
    return acc;
}
