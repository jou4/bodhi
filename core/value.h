#ifndef _value_h_
#define _value_h_


#define T_NIL 0
#define T_CHAR 1
#define T_INT 2
#define T_FLOAT 3
#define T_STRING 4
#define T_TUPLE 5
#define T_LIST 6
#define T_ARRAY 7
#define T_CLOSURE 8

#define TYPE_RANGE_MIN T_NIL
#define TYPE_RANGE_MAX T_CLOSURE

#define SIZE_OF_VALUE sizeof(BDValue)
#define SIZE_OF_ELEMENT sizeof(void *)
#define SIZE_OF_CHAR sizeof(char)

#define DEFAULT_ARRAY_LENGTH 32

#define VALUE_BIT1 'B'
#define VALUE_BIT2 'D'

typedef struct BDValue BDValue;

struct BDValue {
    // Header
    char bit1;
    char bit2;
    char type;
    char age;

    void *forward;

    // Body
    union {
        struct {
            int length;
            void *cell;
        } u_string;
        struct {
            int length;
            void *cell;
        } u_tuple;
        struct {
            void *head;
            BDValue *tail;
        } u_list;
        struct {
            char size_of_elem;
            int length;
            int alloc_length;
            void *cell;
        } u_array;
        struct {
            char *entry;
            int length;
            void *cell;
        } u_closure;
    } u;
};


#define v_bit1(v) v->bit1
#define v_bit2(v) v->bit2
#define v_type(v) v->type
#define v_age(v) v->age
#define v_forward(v) v->forward

#define v_string_length(v) v->u.u_string.length
#define v_string_val(v) v->u.u_string.cell

#define v_tuple_length(v) v->u.u_tuple.length
#define v_tuple_elements(v) v->u.u_tuple.cell

#define v_list_head(v) v->u.u_list.head
#define v_list_tail(v) v->u.u_list.tail

#define v_array_size_of_elem(v) v->u.u_array.size_of_elem
#define v_array_length(v) v->u.u_array.length
#define v_array_max_length(v) v->u.u_array.alloc_length
#define v_array_elements(v) v->u.u_array.cell

#define v_closure_entry(v) v->u.u_closure.entry
#define v_closure_length(v) v->u.u_closure.length
#define v_closure_vars(v) v->u.u_closure.cell


int value_size(char type);
int is_value(BDValue *v);
#define is_expected_value(v, type) (is_value(v) && v_type(v) == type)
void assert_expected_value(BDValue *v, char type);
int is_nil(void *p);

BDValue *bd_value_string(int length, char *val);
BDValue *bd_value_tuple(int length);
BDValue *bd_value_list(void *head, BDValue *tail);
BDValue *bd_value_array(char size_of_elem, int length);
BDValue *bd_value_closure(char *entry, int length);


#endif
