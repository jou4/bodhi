#ifndef _vector_h_
#define _vector_h_

#include <stdio.h>
#include <stdlib.h>

#define VECTOR_SIZE 256

typedef struct {
    void **blocks;
    int nblocks;
    int length;
    int offset;
} Vector;


#define vector_each(vec, f) \
    vector_seek(vec, -1, SEEK_SET); \
    while(vector_has_next(vec)){ \
        f(vector_get_next(vec)); \
    } \
    vector_seek(vec, 0, SEEK_SET);


Vector *vector_new();
void vector_destroy(Vector *vec);
void vector_add(Vector *vec, void *o);
void vector_set(Vector *vec, int index, void *o);
void *vector_get(Vector *vec, int index);
int vector_index_of(Vector *vec, void *o);
Vector *vector_clone(Vector *src);

void vector_seek(Vector *vec, int offset, int whence);
void *vector_get_current(Vector *vec);
void *vector_get_next(Vector *vec);
int vector_has_next(Vector *vec);


#endif
