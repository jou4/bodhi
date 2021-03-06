#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "core.h"
#include "gc.h"

extern void *BASE_PTR;
extern void *STACK_PTR;

void bodhi_core_gc_init(size_t minor_heap_size, size_t major_heap_size)
{
	gc_init(minor_heap_size, major_heap_size);
}

void bodhi_core_gc_finish()
{
	gc_finish();
}

void bodhi_core_push_global_ptr(void *p)
{
	gc_push_global(p);
}

void *bodhi_core_make_closure(char *entry, int num_of_vars)
{
    return bd_value_closure(entry, num_of_vars);
}

void *bodhi_core_closure_entry(BDValue *v)
{
    //assert_expected_value(v, T_CLOSURE);
    if( ! is_expected_value(v, T_CLOSURE)){
        return v;
    }
    else{
        return v_closure_entry(v);
    }
}

void *bodhi_core_closure_vars(BDValue *v)
{
    //assert_expected_value(v, T_CLOSURE);
    if( ! is_expected_value(v, T_CLOSURE)){
        return NULL;
    }
    else{
        return v_closure_vars(v);
    }
}

void *bodhi_core_make_tuple(int num_of_elements)
{
    return bd_value_tuple(num_of_elements);
}

void *bodhi_core_tuple_elems(BDValue *v)
{
    assert_expected_value(v, T_TUPLE);
    return v_tuple_elements(v);
}

void *bodhi_core_make_string(char *val)
{
    return bd_value_string(strlen(val), val);
}

void *bodhi_core_list_cons(void *head, void *tail)
{
    return bd_value_list(head, tail);
}



void bodhi_core_print_str(char *str)
{
	printf("%s\n", str);
}

void bodhi_core_print_ptr(void *p)
{
	printf("%p\n", p);
}
