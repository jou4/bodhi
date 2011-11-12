#include <string.h>
#include <math.h>
#include "gc.h"

GC *gc;

void *BASE_PTR = NULL;
void *STACK_PTR = NULL;

void gc_init(size_t minor_heap_size, size_t major_heap_size)
{
	gc = gc_create(minor_heap_size, major_heap_size);
}

void gc_finish()
{
	gc_destroy(gc);
}

void *gc_allocate(size_t size)
{
	if( ! gc_allocatable_minor_heap(gc, size)){
#ifdef DEBUG
		printf("GC Start.\n");
#endif
		gc_start();

		if( ! gc_allocatable_minor_heap_strict(gc, size)){
			printf("*** Exception: Memory was exhausted.\n");
#ifdef DEBUG
			printf("Try to allocate %lu bytes.", size);
			gc_dump(gc);
#endif
			gc_destroy(gc);
			exit(EXIT_FAILURE);
		}
	}
	return heap_allocate(gc_active_minor_heap(gc), size);
}

void gc_start()
{
	Heap *active = gc_active_minor_heap(gc);
	Heap *deactive = gc_deactive_minor_heap(gc);

	int i;
	Vector *vec;
	PTR *g;

	// globals
	vec = gc_globals(gc);
	for(i = 0; i < vec->length; i++){
		g = vector_get(vec, i);
		*g = (PTR)gc_copy(active, deactive, (BDValue *)*g);
	}

	// args
	vec = gc_args(gc);
	for(i = 0; i < vec->length; i++){
		g = vector_get(vec, i);
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

int is_gc_object(BDValue *target)
{
	return is_heap_object(gc_active_minor_heap(gc), target);
}

void gc_push_global(void *v)
{
	vector_add(gc_globals(gc), v);
}

void gc_push_arg(void *v)
{
	vector_add(gc_args(gc), v);
}

void gc_clear_args()
{
	gc_args(gc)->length = 0;
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
	if( ! is_heap_object(active, target)){
		return target;
	}

	if(v_forward(target) != NULL){
		return v_forward(target);
	}

	BDValue *value = heap_allocate(deactive, sizeof(BDValue));
	*value = *target;
	v_age(value)++;
	v_forward(value) = NULL;
	v_forward(target) = value;

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
	if(heap_block(heap) > p){
		return 0;
	}

	if(heap_block(heap) + heap->size < p){
		return 0;
	}

	return 1;
}

int is_heap_object(Heap *heap, BDValue *target)
{
	return is_exists_in_heap(heap, target) && is_value(target);
}

GC *gc_create(size_t minor_heap_size, size_t major_heap_size)
{
	GC *gc = malloc(sizeof(GC));
	gc_minor_heap_size(gc) = minor_heap_size;
	gc_minor_heap_threashold(gc) = minor_heap_size / 4 * 3;
	gc_major_heap_size(gc) = major_heap_size;
	gc_major_heap_threashold(gc) = major_heap_size / 4 * 3;
	gc_active_switch(gc) = 0;
	gc_minor_heap_1(gc) = heap_create(minor_heap_size);
	gc_minor_heap_2(gc) = heap_create(minor_heap_size);
	gc_major_heap(gc) = heap_create(major_heap_size);
	gc_globals(gc) = vector_new();
	gc_args(gc) = vector_new();
	return gc;
}

void gc_destroy(GC *gc)
{
	heap_destroy(gc_minor_heap_1(gc));
	heap_destroy(gc_minor_heap_2(gc));
	heap_destroy(gc_major_heap(gc));
	vector_destroy(gc_globals(gc));
	vector_destroy(gc_args(gc));
	free(gc);
}

Heap *heap_create(size_t size)
{
	Heap *heap = malloc(sizeof(Heap));
	heap_size(heap) = size;
	heap_used(heap) = 0;
	heap_block(heap) = malloc(size);
	heap_head(heap) = heap_block(heap);
	return heap;
}

void heap_init(Heap *heap)
{
	char *block = heap_block(heap);
	int i, length = heap_size(heap);
	for(i = 0; i < length; i++){
		*block = 0;
		block++;
	}

	heap_used(heap) = 0;
	heap_head(heap) = heap_block(heap);
}

void heap_destroy(Heap *heap)
{
	free(heap_block(heap));
	free(heap);
}

void *heap_allocate(Heap *heap, size_t alloc_size)
{
	if(alloc_size % HEAP_ALIGN != 0){
		alloc_size = (size_t)ceil((double)alloc_size / (double)HEAP_ALIGN) * HEAP_ALIGN;
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

	return heap_allocatable_strict(heap, alloc_size);
}

int heap_allocatable_strict(Heap *heap, size_t alloc_size)
{
	if(heap_used(heap) + alloc_size >= heap_size(heap)){
		return 0;
	}

	return 1;
}

int gc_allocatable_minor_heap(GC *gc, size_t alloc_size)
{
	return heap_allocatable(gc_active_minor_heap(gc), alloc_size, gc_minor_heap_threashold(gc));
}

int gc_allocatable_minor_heap_strict(GC *gc, size_t alloc_size)
{
	return heap_allocatable_strict(gc_active_minor_heap(gc), alloc_size);
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



void heap_dump(Heap *heap, char *name)
{
	printf("%s: (%p)\n", name, heap);
	printf("size: %lu\n", heap_size(heap));
	printf("used: %lu\n", heap_used(heap));
	printf("block: %p\n", heap_block(heap));
	printf("head: %p\n", heap_head(heap));
	printf("\n");
}

void gc_dump(GC *gc)
{
	printf("GC:\n");
	printf("gc_minor_heap_size: %lu\n", gc_minor_heap_size(gc));
	printf("gc_minor_heap_threashold: %lu\n", gc_minor_heap_threashold(gc));
	printf("gc_major_heap_size: %lu\n", gc_major_heap_size(gc));
	printf("gc_major_heap_threashold: %lu\n", gc_major_heap_threashold(gc));
	printf("gc_active_switch: %d\n", gc_active_switch(gc));
	printf("gc_minor_heap_1: %p\n", gc_minor_heap_1(gc));
	printf("gc_minor_heap_2: %p\n", gc_minor_heap_2(gc));
	printf("gc_major_heap: %p\n", gc_major_heap(gc));
	printf("\n");

	heap_dump(gc_active_minor_heap(gc), "Active Heap");
	heap_dump(gc_deactive_minor_heap(gc), "Deactive Heap");
	heap_dump(gc_major_heap(gc), "Major Heap");
}

