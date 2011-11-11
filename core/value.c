#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "value.h"
#include "gc.h"

void *NIL_PTR = "(nil)";

int value_size(char type)
{
    switch(type){
        case T_NIL:
        case T_CHAR:
            return sizeof(char);
        case T_INT:
        case T_FLOAT:
        case T_STRING:
        case T_TUPLE:
        case T_LIST:
        case T_ARRAY:
        case T_CLOSURE:
        default:
            return sizeof(void *);
    }
}

int is_value(BDValue *v)
{
    if(v == NULL){ return 0; }
    if(((long)v % 16) != 0){ return 0; }
    if(v_bit1(v) != VALUE_BIT1){ return 0; }
    if(v_bit2(v) != VALUE_BIT2){ return 0; }
    if(v_type(v) < TYPE_RANGE_MIN || v_type(v) > TYPE_RANGE_MAX){ return 0; }
    return 1;
}

void assert_expected_value(BDValue *v, char type)
{
    if( ! is_expected_value(v, type)){
        printf("*** Exception: Unsupported value was passed.\n");
        exit(EXIT_FAILURE);
    }
}

int is_nil(void *p)
{
    if(p == NIL_PTR){
        return 1;
    }
    return 0;
}

BDValue *bd_value(char type)
{
    BDValue *v = gc_allocate(SIZE_OF_VALUE);
    v_bit1(v) = VALUE_BIT1;
    v_bit2(v) = VALUE_BIT2;
    v_type(v) = type;
    v_age(v) = 0;
    v_forward(v) = NULL;
    return v;
}

BDValue *bd_value_string(int length, char *val)
{
    char *cell = gc_allocate(SIZE_OF_CHAR * (length + 1));
    if(val != NULL){
        strcpy(cell, val);
    }

    BDValue *v = bd_value(T_STRING);
    v_string_length(v) = length;
    v_string_val(v) = cell;
    return v;
}

BDValue *bd_value_tuple(int length)
{
    BDValue *v = bd_value(T_TUPLE);
    v_tuple_length(v) = length;
    v_tuple_elements(v) = gc_allocate(SIZE_OF_ELEMENT * length);
    return v;
}

BDValue *bd_value_list(void *head, BDValue *tail)
{
    BDValue *v = bd_value(T_LIST);
    v_list_head(v) = head;
    v_list_tail(v) = tail;
    return v;
}

BDValue *bd_value_array(char size_of_elem, int length)
{
    int max_length;
    if(length > 0){
        max_length = length;
    }
    else{
        max_length = DEFAULT_ARRAY_LENGTH;
    }

    BDValue *v = bd_value(T_ARRAY);
    v_array_size_of_elem(v) = size_of_elem;
    v_array_length(v) = length;
    v_array_max_length(v) = max_length;
    v_array_elements(v) = gc_allocate(size_of_elem * max_length);
    return v;
}

BDValue *bd_value_closure(char *entry, int length)
{
    BDValue *v = bd_value(T_CLOSURE);
    v_closure_entry(v) = entry;
    v_closure_length(v) = length;
    v_closure_vars(v) = gc_allocate(SIZE_OF_ELEMENT * length);
    return v;
}
