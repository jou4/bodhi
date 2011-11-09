#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char o;
    char *entry;
} BDValueClosure;

void *bodhi_core_make_closure(char *entry, int fvs_size)
{
    BDValueClosure *value = malloc(sizeof(BDValueClosure) + fvs_size);
    value->o = 'O';
    value->entry = entry;
    return value;
}

void *bodhi_core_closure_entry(void *value)
{
    BDValueClosure *cls = (BDValueClosure *)value;
    if(cls->o == 'O'){
        return cls->entry;
    }
    return value;
}

void *bodhi_core_closure_freevars(void *value)
{
    return value + sizeof(BDValueClosure);
}

typedef struct {
    char o;
} BDValueTuple;

void *bodhi_core_make_tuple(int size)
{
    BDValueTuple *value = malloc(sizeof(BDValueTuple) + sizeof(void *) * size);
    value->o = 'O';
    return value;
}

void *bodhi_core_tuple_elems(void *value)
{
    return value + sizeof(BDValueTuple);
}

typedef struct {
    char o;
} BDValueString;

void *bodhi_core_make_string(char *str)
{

    void *value = malloc(sizeof(BDValueString) + strlen(str) + 1);
    ((BDValueString *)value)->o = 'O';

    char *cell = value + sizeof(BDValueString);
    strcpy(cell, str);

    return value;
}




void bodhi_print_int(int x)
{
    printf("%d\n", x);
}

void bodhi_print_float(double x)
{
    printf("%.14g\n", x);
}

void bodhi_print_char(char x)
{
    printf("%c\n", x);
}

void bodhi_print_string(void *x)
{
    char *str = x + sizeof(BDValueString);
    printf("%s\n", str);
}

