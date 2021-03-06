#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "core.h"
#include "gc.h"

extern void *NIL_PTR;

double bodhi_i2f(long v);
long bodhi_f2i(double v);

char bodhi_i2c(int v)
{
    return (char)v;
}

int bodhi_c2i(char v)
{
    return (int)v;
}

void *bodhi_head(BDValue *v)
{
    if(is_nil(v)){
        printf("*** Exception: head: Empty list was passed.\n");
        exit(EXIT_FAILURE);
    }

    assert_expected_value(v, T_LIST);
    return v_list_head(v);
}

void *bodhi_tail(BDValue *v)
{
    if(is_nil(v)){
        printf("*** Exception: tail: Empty list was passed.\n");
        exit(EXIT_FAILURE);
    }

    assert_expected_value(v, T_LIST);
    return v_list_tail(v);
}

int bodhi_empty(BDValue *v)
{
    if(is_nil(v)){
        return 1;
    }
    return 0;
}

void *bodhi_array_make(int length, void *init_element)
{
    BDValue *v = bd_value_array(SIZE_OF_ELEMENT, length);
    PTR *cell = v_array_elements(v);

    int i;
    for(i = 0; i < length; i++){
        *cell = (PTR)init_element;
        cell++;
    }

    return v;
}

void bodhi_array_set(BDValue *v, int index, void *element)
{
    assert_expected_value(v, T_ARRAY);

    PTR *cell = v_array_elements(v);
    cell += index;

    *cell = (PTR)element;
}

void *bodhi_array_get(BDValue *v, int index)
{
    assert_expected_value(v, T_ARRAY);

    PTR *cell = v_array_elements(v);
    cell += index;

    return (void *)*cell;
}

int bodhi_array_length(BDValue *v)
{
    assert_expected_value(v, T_ARRAY);
    return v_array_length(v);
}

int _bodhi_list_length(BDValue *v)
{
	int length = 0;
	BDValue *tmp = v;
	while( ! is_nil(tmp)){
		length++;
		tmp = v_list_tail(tmp);
	}
	return length;
}

void *bodhi_array_of_list(BDValue *v)
{
    assert_expected_value(v, T_LIST);
	GC_PUSH_ARG(v);

	int length = _bodhi_list_length(v);
	BDValue *ary = bd_value_array(SIZE_OF_ELEMENT, length);

	BDValue *tmp = v;
	PTR *cell = v_array_elements(ary);
	while( ! is_nil(tmp)){
		*cell = (PTR)v_list_head(tmp);
		tmp = v_list_tail(tmp);
		cell++;
	}

	GC_CLEAR_ARGS();
	return ary;
}

void *bodhi_array_to_list(BDValue *v)
{
    assert_expected_value(v, T_ARRAY);
	GC_PUSH_ARG(v);

	int i, length = v_array_length(v);
	PTR *cell = v_array_elements(v);
	cell += (length - 1);

	BDValue *list = NIL_PTR;

	for(i = length - 1; i >= 0; i--){
		list = bd_value_list((void *)*cell, list);
		GC_PUSH_ARG(list);
		cell--;
	}

	GC_CLEAR_ARGS();
	return list;
}

void *bodhi_string_make(int length)
{
    return bd_value_string(length, NULL);
}

void bodhi_string_set(BDValue *v, int index, char c)
{
    assert_expected_value(v, T_STRING);

    char *cell = v_string_val(v);
    cell += index;

    *cell = c;
}

char bodhi_string_get(BDValue *v, int index)
{
    assert_expected_value(v, T_STRING);

    char *cell = v_string_val(v);
    cell += index;

    return *cell;
}

int bodhi_string_length(BDValue *v)
{
    assert_expected_value(v, T_STRING);
    return v_string_length(v);
}

void *bodhi_string_of_cstr(char *val)
{
    return bodhi_core_make_string(val);
}

char *bodhi_string_to_cstr(BDValue *v)
{
    assert_expected_value(v, T_STRING);
    return v_string_val(v);
}

char *_bodhi_string_copy(char *s1, char *s2)
{
    while(*s2 > 0){
        *s1 = *s2;
        s1++;
        s2++;
    }

    return s1;
}

void *bodhi_string_append(BDValue *v1, BDValue *v2)
{
    assert_expected_value(v1, T_STRING);
    assert_expected_value(v2, T_STRING);
	GC_PUSH_ARG(v1);
	GC_PUSH_ARG(v2);

    BDValue *v = bd_value_string(v_string_length(v1) + v_string_length(v2), NULL);
    char *cell = v_string_val(v);

    cell = _bodhi_string_copy(cell, v_string_val(v1));
    cell = _bodhi_string_copy(cell, v_string_val(v2));
    *cell = '\0';

	GC_CLEAR_ARGS();
    return v;
}

void *bodhi_string_concat(BDValue *sep, BDValue *list)
{
    assert_expected_value(sep, T_STRING);
    assert_expected_value(list, T_LIST);
	GC_PUSH_ARG(sep);
	GC_PUSH_ARG(list);

    int i, result_length = 0, sep_length = v_string_length(sep);
    BDValue *tmp, *str;

    i = 0;
    tmp = list;
    while( ! is_nil(tmp)){
        assert_expected_value(v_list_head(tmp), T_STRING);
        if(i++ > 0){
            result_length += sep_length;
        }
        str = v_list_head(tmp);
        result_length += v_string_length(str);
        tmp = v_list_tail(tmp);
    }

    BDValue *v = bd_value_string(result_length, NULL);
    char *cell = v_string_val(v);

    i = 0;
    tmp = list;
    while( ! is_nil(tmp)){
        if(i++ > 0){
            cell = _bodhi_string_copy(cell, v_string_val(sep));
        }
        str = v_list_head(tmp);
        cell = _bodhi_string_copy(cell, v_string_val(str));
        tmp = v_list_tail(tmp);
    }
    *cell = '\0';

	GC_CLEAR_ARGS();
    return v;
}

int bodhi_string_compare(BDValue *v1, BDValue *v2)
{
    assert_expected_value(v1, T_STRING);
    assert_expected_value(v2, T_STRING);

    if(v_string_length(v1) != v_string_length(v2)){
        return 0;
    }

    char *cell1 = v_string_val(v1);
    char *cell2 = v_string_val(v2);

	int i, length = v_string_length(v1);
	for(i = 0; i < length; i++){
        if(*cell1 != *cell2){
            return 0;
        }
        cell1++;
        cell2++;
	}

    return 1;
}

void *bodhi_ref(void *val)
{
	GC_PUSH_ARG(val);
    BDValue *v = bodhi_array_make(1, val);
	GC_CLEAR_ARGS();
	return v;
}

void *bodhi_deref(BDValue *v)
{
    if( ! is_value(v) || v_type(v) != T_ARRAY || v_array_length(v) != 1){
        printf("*** Exception: deref: Unsupported value was passed.\n");
        exit(EXIT_FAILURE);
    }

    return bodhi_array_get(v, 0);
}

void bodhi_setref(BDValue *v, void *val)
{
    if( ! is_value(v) || v_type(v) != T_ARRAY || v_array_length(v) != 1){
        printf("*** Exception: setref: Unsupported value was passed.\n");
        exit(EXIT_FAILURE);
    }

    return bodhi_array_set(v, 0, val);
}


void bodhi_print_bool(int v)
{
    if(v == 0){
        printf("False\n");
    }
    else{
        printf("True\n");
    }
}

#define BREAKLINE printf("\n")
void _bodhi_print(BDValue *v);

void _bodhi_print_int(long v)
{
	printf("%ld", v);
}

void _bodhi_print_float(double v)
{
    printf("%.14g", v);
}

void _bodhi_print_char(char v)
{
    printf("%c", v);
}

void _bodhi_print_string(BDValue *v)
{
    printf("%s", (char *)v_string_val(v));
}

void _bodhi_print_nil()
{
    printf("[]");
}

void _bodhi_print_tuple(BDValue *v)
{
    int i, length = v_tuple_length(v);
    PTR *elements = v_tuple_elements(v);

    printf("(");
    for(i = 0; i < length; i++){
        if(i > 0){
            printf(", ");
        }
        _bodhi_print((BDValue *)*elements);
        elements++;
    }
    printf(")");
}

void _bodhi_print_list_rec(BDValue *v, int index)
{
    if(index > 0){
        printf(", ");
    }
    _bodhi_print(v_list_head(v));
    if( ! is_nil(v_list_tail(v))){
        _bodhi_print_list_rec(v_list_tail(v), ++index);
    }
}

void _bodhi_print_list(BDValue *v)
{
    printf("[");
    _bodhi_print_list_rec(v, 0);
    printf("]");
}

void _bodhi_print_array(BDValue *v)
{
    int i, length = v_array_length(v);
    PTR *elements = v_array_elements(v);

    printf("[| ");
    for(i = 0; i < length; i++){
        if(i > 0){
            printf(", ");
        }
        _bodhi_print((BDValue *)*elements);
        elements++;
    }
    printf(" |]");
}

void _bodhi_print(BDValue *v)
{
	if(is_nil(v)){
		_bodhi_print_nil(); return;
	}

	if(is_gc_object(v)){
        switch(v_type(v)){
            case T_STRING:
                _bodhi_print_string(v);
                break;
            case T_TUPLE:
                _bodhi_print_tuple(v);
                break;
            case T_LIST:
                _bodhi_print_list(v);
                break;
            case T_ARRAY:
                _bodhi_print_array(v);
                break;
            case T_CLOSURE:
                printf("(closure)");
                break;
            default:
                _bodhi_print_int((long)v);
                break;
        }
    }
    else{
        _bodhi_print_int((long)v);
    }
}


void bodhi_print(BDValue *v)
{
    _bodhi_print(v); BREAKLINE;
}

void bodhi_print_int(long v)
{
    _bodhi_print_int(v); BREAKLINE;
}

void bodhi_print_float(double v)
{
    _bodhi_print_float(v); BREAKLINE;
}

void bodhi_print_char(char v)
{
    _bodhi_print_char(v); BREAKLINE;
}

void bodhi_print_string(BDValue *v)
{
    _bodhi_print_string(v); BREAKLINE;
}

void bodhi_print_tuple(BDValue *v)
{
    _bodhi_print_tuple(v); BREAKLINE;
}

void bodhi_error(BDValue *v)
{
    printf("*** Exception: %s\n", (char *)v_string_val(v));
    exit(EXIT_FAILURE);
}
