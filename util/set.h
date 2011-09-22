#ifndef _set_h_
#define _set_h_

#include <stdlib.h>
#include "map.h"
#include "vector.h"

typedef Map Set;

Set *set_new();
void set_destroy(Set *set);

void set_add(Set *set, const char *val);
void set_remove(Set *set, const char *val);
int set_has(Set *set, const char *val);
int set_is_empty(Set *set);
Set *set_clone(Set *src);
Set *set_diff(Set *x, Set *y);
Set *set_union(Set *x, Set *y);
Vector *set_elements(Set *set);
Set *set_of_list(Vector *vec);

#endif
