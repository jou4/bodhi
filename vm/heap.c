#include <stdio.h>
#include "heap.h"

Heap *heap_create(size_t size, size_t threshold, void *top)
{
    Heap *heap = malloc(sizeof(Heap));
    heap->size = size;
    heap->threshold = threshold;
    heap->top = top;
    heap->current = top;
    heap->use = 0;
    return heap;
}

void heap_destroy(Heap *heap)
{
    free(heap);
}

void heap_init(Heap *heap)
{
    heap->current = heap->top;
    heap->use = 0;
}

void *heap_alloc(Heap *heap, size_t size)
{
    return heap_realloc(heap, NULL, 0, size);
}

void *heap_realloc(Heap *heap, void *p, size_t old_size, size_t new_size)
{
    void *ret;
    size_t use = heap->use + new_size;

    if(use > heap->size){
        return NULL;
    }

    ret = heap->current;
    heap->current += new_size;
    heap->use = use;
    return ret;
}

void heap_free(Heap *heap, void *p, size_t size)
{
}

int heap_over_threshold(Heap *heap)
{
    if(heap->use > heap->threshold){
        return 1;
    }
    return 0;
}


/*
static char area[1024];

typedef struct {
    int a;
    int b;
} Test;

void fill_char(char *str, int size)
{
    int i;
    for(i = 0; i < size - 2; i++){
        str[i] = 'A';
    }
    str[size - 1] = '\0';
}

void check(char *str, int size)
{
    if(str == NULL){
        printf("exhausted memory!!\n");
        return;
    }
    fill_char(str, 300);
    printf("%p: %s\n", str, str);
}

int main()
{
    Heap *heap = heap_create(1024, 800, area);

    char *str1 = heap_alloc(heap, 300);
    check(str1, 300);

    if(heap_over_threshold(heap)){
        printf("over threshold. Do gc!\n");
    }

    char *str2 = heap_alloc(heap, 300);
    check(str2, 300);

    if(heap_over_threshold(heap)){
        printf("over threshold. Do gc!\n");
    }

    char *str3 = heap_alloc(heap, 300);
    check(str3, 300);

    if(heap_over_threshold(heap)){
        printf("over threshold. Do gc!\n");
    }

    char *str4 = heap_alloc(heap, 125);
    check(str4, 125);

    Test *t1 = heap_alloc(heap, sizeof(Test));
    t1->a = 10;
    t1->b = 20;

    Test *t2 = heap_alloc(heap, sizeof(Test));
    t2->a = 30;
    t2->b = 40;

    printf("%p: a=%d, b=%d\n", t1, t1->a, t1->b);
    printf("%p: a=%d, b=%d\n", t2, t2->a, t2->b);

    heap_destroy(heap);
    return 0;
}
*/
