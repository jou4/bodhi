#include <stdlib.h>
#include <string.h>
#include "mem.h"

char *mem_strdup(const char *src)
{
    if(src == NULL){ return NULL; }

    int len = strlen(src);
    char *dst = (char *)malloc(sizeof(char) * len + 1);
    strcpy(dst, src);
    return dst;
}
