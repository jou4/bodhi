#ifndef _gc_h_
#define _gc_h_

#include "value.h"
#include "util.h"

#define HEAP_ALIGN 8

typedef struct {
	size_t size;
	size_t used;
	void *block;
	void *head;
} Heap;

typedef struct {
	size_t minor_heap_size;
	size_t minor_heap_threashold;
	size_t major_heap_size;
	size_t major_heap_threashold;
	int active_switch;
	Heap *minor_heap_1;
	Heap *minor_heap_2;
	Heap *major_heap;
} GC;


void gc_init(size_t minor_heap_size, size_t major_heap_size);
void *gc_allocate(size_t size);
void gc_start();
void *gc_copy(Heap *active, Heap *deactive, BDValue *target);
int is_exists_in_heap(Heap *heap, void *p);
int is_gc_object(Heap *heap, BDValue *target);

#define gc_minor_heap_size(gc) gc->minor_heap_size
#define gc_minor_heap_threashold(gc) gc->minor_heap_threashold
#define gc_major_heap_size(gc) gc->major_heap_size
#define gc_major_heap_threashold(gc) gc->major_heap_threashold
#define gc_active_switch(gc) gc->active_switch
#define gc_minor_heap_1(gc) gc->minor_heap_1
#define gc_minor_heap_2(gc) gc->minor_heap_2
#define gc_major_heap(gc) gc->major_heap

GC *gc_create(size_t minor_heap_size, size_t major_heap_size);
void gc_destory(GC *gc);

int gc_allocatable_minor_heap(GC *gc, size_t alloc_size);
int gc_allocatable_major_heap(GC *gc, size_t alloc_size);
void gc_switch_minor_heap(GC *gc);
Heap *gc_active_minor_heap(GC *gc);
Heap *gc_deactive_minor_heap(GC *gc);


#define heap_size(heap) heap->size
#define heap_used(heap) heap->used
#define heap_block(heap) heap->block
#define heap_head(heap) heap->head

Heap *heap_create(size_t size);
void heap_init(Heap *heap);
void heap_destory(Heap *heap);

void *heap_allocate(Heap *heap, size_t alloc_size);
int heap_allocatable(Heap *heap, size_t alloc_size, size_t threashold);


#endif
