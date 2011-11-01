#include <stdio.h>
#include "show.h"

void bd_show_indent(int depth)
{
    int i;
    for(i = 0; i < depth; i++){
        printf("  ");
    }
}
