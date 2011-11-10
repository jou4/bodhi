#ifndef _core_h_
#define _core_h_

#include "value.h"

void *bodhi_core_make_closure(char *entry, int num_of_vars);
void *bodhi_core_closure_entry(BDValue *v);
void *bodhi_core_closure_vars(BDValue *v);
void *bodhi_core_make_tuple(int num_of_elements);
void *bodhi_core_tuple_elems(BDValue *v);
void *bodhi_core_make_string(char *val);
void *bodhi_core_list_cons(void *head, void *tail);

#endif
