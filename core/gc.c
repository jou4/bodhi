#include <string.h>
#include "gc.h"

GC *gc;

void *BASE_PTR = NULL;
void *STACK_PTR = NULL;
Vector *globals;

void gc_init(size_t minor_heap_size, size_t major_heap_size)
{
	gc = gc_create(minor_heap_size, major_heap_size);
	globals = vector_new();
}

void *gc_allocate(size_t size)
{
	if( ! gc_allocatable_minor_heap(gc, size)){
		gc_start();
	}
	return heap_allocate(gc_active_minor_heap(gc), size);
}

void gc_start()
{
	Heap *active = gc_active_minor_heap(gc);
	Heap *deactive = gc_deactive_minor_heap(gc);

	// globals
	int i;
	PTR *g;
	for(i = 0; i < globals->length; i++){
		g = vector_get(globals, i);
		*g = (PTR)gc_copy(active, deactive, (BDValue *)*g);
	}

	// stack
	PTR *bp = BASE_PTR;
	PTR *sp = STACK_PTR;
	while(sp < bp){
		*sp = (PTR)gc_copy(active, deactive, (BDValue *)*sp);
		sp++;
	}

	heap_init(active);
	gc_switch_minor_heap(gc);
}

void cell_copy(Heap *active, Heap *deactive, PTR *dst, PTR *src, int length)
{
	int i;
	for(i = 0; i < length; i++){
		*dst = (PTR)gc_copy(active, deactive, (BDValue *)*src);
		dst++;
		src++;
	}
}

void *gc_copy(Heap *active, Heap *deactive, BDValue *target)
{
	if( ! is_gc_object(active, target)){
		return target;
	}

	if(v_forward(target) != NULL){
		return v_forward(target);
	}

	BDValue *value = heap_allocate(deactive, sizeof(BDValue));
	*value = *target;
	v_age(target)++;
	v_forward(target) = NULL;

	switch(target->type){
		case T_STRING:
			{
				char *cell = heap_allocate(deactive, SIZE_OF_CHAR * (v_string_length(target) + 1));
				strcpy(cell, v_string_val(target));
				v_string_val(value) = cell;
			}
			break;
		case T_TUPLE:
			v_tuple_elements(value) = heap_allocate(deactive, SIZE_OF_ELEMENT * v_tuple_length(target));
			cell_copy(active, deactive,
					v_tuple_elements(value), v_tuple_elements(target), v_tuple_length(target));
			break;
		case T_LIST:
			v_list_head(value) = gc_copy(active, deactive, v_list_head(target));
			v_list_tail(value) = gc_copy(active, deactive, v_list_tail(target));
			break;
		case T_ARRAY:
			v_array_elements(value) = heap_allocate(deactive, SIZE_OF_ELEMENT * v_array_max_length(target));
			cell_copy(active, deactive,
					v_array_elements(value), v_array_elements(target), v_array_length(target));
			break;
		case T_CLOSURE:
			v_closure_entry(value) = gc_copy(active, deactive, v_closure_entry(target));
			v_closure_vars(value) = heap_allocate(deactive, SIZE_OF_ELEMENT * v_closure_length(target));
			cell_copy(active, deactive,
					v_closure_vars(value), v_closure_vars(target), v_closure_length(target));
			break;
		default:
			printf("*** Exception: Unknown object was passed.\n");
			exit(EXIT_FAILURE);
			break;
	}

	return value;
}

int is_exists_in_heap(Heap *heap, void *p)
{
	if(heap_block(heap) >= p){
		return 0;
	}

	if(heap_block(heap) + heap->size < p){
		return 0;
	}

	return 1;
}

int is_gc_object(Heap *heap, BDValue *target)
{
	return is_value(target) && is_exists_in_heap(heap, target);
}

GC *gc_create(size_t minor_heap_size, size_t major_heap_size)
{
	GC *gc = malloc(sizeof(GC));
	gc_minor_heap_size(gc) = minor_heap_size;
	gc_minor_heap_threashold(gc) = minor_heap_size / 3 * 2;
	gc_major_heap_size(gc) = major_heap_size;
	gc_major_heap_threashold(gc) = major_heap_size / 3 * 2;
	gc_active_switch(gc) = 0;
	gc_minor_heap_1(gc) = heap_create(minor_heap_size);
	gc_minor_heap_2(gc) = heap_create(minor_heap_size);
	gc_major_heap(gc) = heap_create(major_heap_size);
	return gc;
}

void gc_destory(GC *gc)
{
	heap_destory(gc_minor_heap_1(gc));
	heap_destory(gc_minor_heap_2(gc));
	heap_destory(gc_major_heap(gc));
	free(gc);
}

Heap *heap_create(size_t size)
{
	Heap *heap = malloc(sizeof(Heap));
	heap_size(heap) = size;
	heap_used(heap) = 0;
	heap_block(heap) = malloc(sizeof(size));
	heap_head(heap) = heap_block(heap);
	return heap;
}

void heap_init(Heap *heap)
{
	heap_used(heap) = 0;
	heap_head(heap) = heap_block(heap);
}

void heap_destory(Heap *heap)
{
	free(heap_block(heap));
	free(heap);
}

void *heap_allocate(Heap *heap, size_t alloc_size)
{
	if(alloc_size % HEAP_ALIGN != 0){
		alloc_size = alloc_size / HEAP_ALIGN + HEAP_ALIGN;
	}

	void *m = heap_head(heap);
	heap_used(heap) += alloc_size;
	heap_head(heap) += alloc_size;

	return m;
}

int heap_allocatable(Heap *heap, size_t alloc_size, size_t threashold)
{
	if(heap_used(heap) > threashold){
		return 0;
	}

	if(heap_used(heap) + alloc_size >= heap_size(heap)){
		return 0;
	}

	return 1;
}

int gc_allocatable_minor_heap(GC *gc, size_t alloc_size)
{
	return heap_allocatable(gc_active_minor_heap(gc), alloc_size, gc_minor_heap_threashold(gc));
}

int gc_allocatable_major_heap(GC *gc, size_t alloc_size)
{
	return heap_allocatable(gc_major_heap(gc), alloc_size, gc_major_heap_threashold(gc));
}

void gc_switch_minor_heap(GC *gc)
{
	gc_active_switch(gc) = ! gc_active_switch(gc);
}

Heap *gc_active_minor_heap(GC *gc)
{
	if(gc_active_switch(gc)){
		return gc_minor_heap_2(gc);
	}
	return gc_minor_heap_1(gc);
}

Heap *gc_deactive_minor_heap(GC *gc)
{
	if(gc_active_switch(gc)){
		return gc_minor_heap_1(gc);
	}
	return gc_minor_heap_2(gc);
}
