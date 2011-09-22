#ifndef _list_h_
#define _list_h_

#include <stdlib.h>

typedef struct List List;

struct List {
    void *head;
    List *tail;
};


List *list_cons(void *head, List *tail);
void list_destroy(List *list);

void *list_get(List *list, int index);
List *list_reverse(List *list);
int list_index_of(List *list, void *val);

#define list_head(list) list->head
#define list_tail(list) list->tail

#endif
