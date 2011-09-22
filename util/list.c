#include <stdio.h>
#include "list.h"

List *list_cons(void *head, List *tail)
{
    List *list = (List *)malloc(sizeof(List));
    list->head = head;
    list->tail = tail;
    return list;
}

void list_destroy(List *list)
{
    List *prev;
    while(list){
        prev = list;
        list = list->tail;
        free(prev);
    }
}

void *list_get(List *list, int index)
{
    for(; index > 0; index--){
        list = list->tail;
    }

    if(list != NULL){
        return list->head;
    }
    return NULL;
}

List *list_reverse(List *list)
{
    List *newlist = NULL;
    while(list){
        newlist = list_cons(list->head, newlist);
        list = list->tail;
    }
    return newlist;
}

List *list_map(List *list, void *(*fp) (void *))
{
    List *newlist = NULL;
    List *result = NULL;
    while(list){
        newlist = list_cons((*fp)(list->head), newlist);
        list = list->tail;
    }
    result = list_reverse(newlist);
    list_destroy(newlist);
    return result;
}

int list_index_of(List *list, void *val)
{
    int index = 0;

    while(list){
        if(list->head == val){
            return index;
        }
        list = list->tail;
        index++;
    }

    return -1;
}

/*
void *_map_f(void *elem)
{
    return "1";
}

int main()
{
    List *list, *target;

    char *str = (char *)malloc(sizeof(char) * 2);
    str[0] = 'z';
    str[1] = '\0';

    list = list_cons("a", NULL);
    list = list_cons("b", list);
    list = list_cons("c", list);
    list = list_cons(str, list);
    list = list_cons("d", list);
    list = list_cons("e", list);

    printf("index of 'str' is %d\n", list_index_of(list, str));
    printf("'str' is \"%s\"\n", (char *)list_get(list, list_index_of(list, str)));

    target = list;
    while(target){
        printf("%s\n", (char *)list_head(target));
        target = list_tail(target);
    }

    list = list_reverse(list);
    int i;
    for(i = 0; i < 5; i++){
        printf("%d, %s\n", i, (char *)list_get(list, i));
    }

    list = list_map(list, _map_f);
    for(i = 0; i < 5; i++){
        printf("%d, %s\n", i, (char *)list_get(list, i));
    }

    list_destroy(list);

    return 0;
}
*/
