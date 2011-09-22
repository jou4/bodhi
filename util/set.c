#include <stdio.h>
#include "set.h"

Set *set_new()
{
    return map_new();
}

void set_destroy(Set *set)
{
    map_destroy(set);
}

void set_add(Set *set, const char *val)
{
    map_set(set, val, NULL);
}

void set_remove(Set *set, const char *val)
{
    map_remove(set, val);
}

int set_has(Set *set, const char *val)
{
    return map_has(set, val);
}

int set_is_empty(Set *set)
{
    Vector *keys = map_keys(set);
    int empty;
    if(keys->length == 0){
        empty = 1;
    }
    else{
        empty = 0;
    }
    vector_destroy(keys);
    return empty;
}

Set *set_clone(Set *src)
{
    return map_clone(src);
}

Set *set_diff(Set *x, Set *y)
{
    Set *z = map_clone(x);
    Vector *keys = map_keys(y);
    int i;

    for(i = 0; i < keys->length; i++){
        set_remove(z, vector_get(keys, i));
    }

    vector_destroy(keys);

    return z;
}

Set *set_union(Set *x, Set *y)
{
    Set *z = map_clone(x);
    Vector *keys = map_keys(y);
    int i;

    for(i = 0; i < keys->length; i++){
        set_add(z, vector_get(keys, i));
    }

    vector_destroy(keys);

    return z;
}

Vector *set_elements(Set *set)
{
    return map_keys(set);
}

Set *set_of_list(Vector *vec)
{
    Set *s = set_new();

    int i;
    for(i = 0; i < vec->length; i++){
        set_add(s, vector_get(vec, i));
    }

    return s;
}


/*
int main()
{
    Set *x = set_new();
    Set *y = set_new();

    set_add(x, "a");
    set_add(x, "b");
    set_add(x, "c");
    set_remove(x, "c");

    set_add(y, "a");
    set_add(y, "c");

    Set *z = set_diff(x, y);
    Vector *elems = set_elements(z);

    set_destroy(x);
    set_destroy(y);

    int i;
    char *str;
    for(i = 0; i < elems->length; i++){
        str = vector_get(elems, i);
        printf("%s\n", str);
    }

    set_destroy(z);
}
*/
