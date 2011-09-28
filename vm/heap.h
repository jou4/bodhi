#ifndef _heap_h_
#define _heap_h_

#include <stdlib.h>

typedef struct {
    size_t size;
    size_t threshold;
    void *top;
    void *current;
    size_t use;
} Heap;

Heap *heap_create(size_t size, size_t threshold, void *top);
void heap_destroy(Heap *heap);
void heap_init(Heap *heap);
void *heap_alloc(Heap *heap, size_t size);
void *heap_realloc(Heap *heap, void *p, size_t old_size, size_t new_size);
void heap_free(Heap *heap, void *p, size_t size);
int  heap_over_threshold(Heap *heap);

#endif
