#include "vector.h"

void vector_resize(Vector *vec, int n)
{
    vec->blocks = realloc(vec->blocks, sizeof(void *) * n);
    vec->nblocks = n;
}

Vector *vector_new()
{
    Vector *vec = (Vector *)malloc(sizeof(Vector));
    vec->blocks = NULL;
    vec->nblocks = 0;
    vec->length = 0;
    vec->offset = 0;
    return vec;
}

void vector_destroy(Vector *vec)
{
    free(vec);
}

void vector_add(Vector *vec, void *o)
{
    if(vec->nblocks <= vec->length){
        vector_resize(vec, (vec->nblocks == 0) ? VECTOR_SIZE : vec->nblocks * 2);
    }
    vec->blocks[vec->length++] = o;
}

void vector_set(Vector *vec, int index, void *o)
{
    int need_len = index + 1;
    if(vec->nblocks <= need_len){
        vector_resize(vec, need_len);
    }

    vec->blocks[index] = o;

    if(vec->length < need_len){
        vec->length = need_len;
    }
}

void *vector_get(Vector *vec, int index)
{
    return (vec->nblocks > index) ? vec->blocks[index] : NULL;
}

int vector_index_of(Vector *vec, void *o)
{
    int i;
    for(i = 0; i < vec->length; i++){
        if(vector_get(vec, i) == o){
            return i;
        }
    }
    return -1;
}

Vector *vector_clone(Vector *src)
{
    Vector *dest = vector_new();
    int i;

    for(i = 0; i < src->length; i++){
        vector_add(dest, vector_get(src, i));
    }

    return dest;
}

void vector_seek(Vector *vec, int offset, int whence)
{
    switch(whence){
        case SEEK_SET:
            vec->offset = offset;
            break;
        case SEEK_CUR:
            vec->offset += offset;
            break;
        case SEEK_END:
            vec->offset = vec->length + offset;
            break;
    }
}

void *vector_get_current(Vector *vec)
{
    if(vec->offset < vec->length){
        return vector_get(vec, vec->offset);
    }
    return NULL;
}

void *vector_get_next(Vector *vec)
{
    vector_seek(vec, 1, SEEK_CUR);
    return vector_get_current(vec);
}

int vector_has_next(Vector *vec)
{
    if(vec->offset + 1 < vec->length){
        return 1;
    }
    else{
        return 0;
    }
}


/*
int main()
{
    char *str;
    Vector *vec = vector_new();

    str = malloc(sizeof(char) * 10);
    str[0] = 'a';
    str[1] = '\0';
    vector_add(vec, str);

    str = malloc(sizeof(char) * 10);
    str[0] = 'b';
    str[1] = '\0';
    vector_add(vec, str);

    vector_each(vec, free);
    vector_destroy(vec);

    printf("%s\n", str);

    return 0;
}
*/
