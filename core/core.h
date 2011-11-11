#ifndef _core_h_
#define _core_h_

#include "value.h"

void bodhi_core_gc_init(size_t minor_heap_size, size_t major_heap_size);
void bodhi_core_gc_finish();
void bodhi_core_push_global_ptr(void *p);
void *bodhi_core_make_closure(char *entry, int num_of_vars);
void *bodhi_core_closure_entry(BDValue *v);
void *bodhi_core_closure_vars(BDValue *v);
void *bodhi_core_make_tuple(int num_of_elements);
void *bodhi_core_tuple_elems(BDValue *v);
void *bodhi_core_make_string(char *val);
void *bodhi_core_list_cons(void *head, void *tail);

#endif
