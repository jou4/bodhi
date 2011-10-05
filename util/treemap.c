#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "treemap.h"

TreeMap *treemap_new(TreeMapColor color, void *val, TreeMap *l, TreeMap *r)
{
    TreeMap *map = malloc(sizeof(TreeMap));
    map->color = color;
    map->val = val;
    map->l = l;
    map->r = r;
    return map;
}

TreeMap *treemap_empty()
{
    return NULL;
}

int treemap_is_empty(TreeMap *map)
{
    if(map == NULL){
        return 1;
    }
    return 0;
}

int treemap_is_black(TreeMap *map)
{
    if(map->color == TM_BLACK){
        return 1;
    }
    return 0;
}

int treemap_is_red(TreeMap *map)
{
    if(map->color == TM_RED){
        return 1;
    }
    return 0;
}

TreeMap *treemap_balance(TreeMapColor color, void *v1, TreeMap *l1, TreeMap *r1)
{
    if(color == TM_BLACK){

        if( ! treemap_is_empty(l1) && treemap_is_red(l1)){
            void *v2 = l1->val;
            TreeMap *l2 = l1->l;
            TreeMap *r2 = l1->r;

            if( ! treemap_is_empty(l2) && treemap_is_red(l2)){
                return treemap_new(TM_RED, v2,
                        treemap_new(TM_BLACK, l2->val, l2->l, l2->r),
                        treemap_new(TM_BLACK, v1, r2, r1));
            }
            else if( ! treemap_is_empty(r2) && treemap_is_red(r2)){
                return treemap_new(TM_RED, r2->val,
                        treemap_new(TM_BLACK, v2, l2, r2->l),
                        treemap_new(TM_BLACK, v1, r2->r, r1));
            }
        }

        if( ! treemap_is_empty(r1) && treemap_is_red(r1)){
            void *v2 = r1->val;
            TreeMap *l2 = r1->l;
            TreeMap *r2 = r1->r;

            if( ! treemap_is_empty(l2) && treemap_is_red(l2)){
                return treemap_new(TM_RED, l2->val,
                        treemap_new(TM_BLACK, v1, l1, l2->l),
                        treemap_new(TM_BLACK, v2, l2->r, r2));
            }
            else if( ! treemap_is_empty(r2) && treemap_is_red(r2)){
                return treemap_new(TM_RED, v2,
                        treemap_new(TM_BLACK, v1, l1, l2),
                        treemap_new(TM_BLACK, r2->val, r2->l, r2->r));
            }
        }

    }

    return treemap_new(color, v1, l1, r1);
}

TreeMap *_treemap_insert(TreeMap *map, void *x, TreeMapCmpFunc cmp)
{
    if(treemap_is_empty(map)){
        return treemap_new(TM_RED, x, NULL, NULL);
    }

    TreeMapColor color = map->color;
    void *y = map->val;
    TreeMap *l = map->l;
    TreeMap *r = map->r;

    int cmp_result = cmp(x, y);
    if(cmp_result < 0){
        return treemap_balance(color, y, _treemap_insert(l, x, cmp), r);
    }
    else if(cmp_result > 0){
        return treemap_balance(color, y, l, _treemap_insert(r, x, cmp));
    }
    else{
        return map;
    }
}

TreeMap *treemap_insert(TreeMap *map, void *x, TreeMapCmpFunc cmp)
{
    TreeMap *newmap = _treemap_insert(map, x, cmp);
    newmap->color = TM_BLACK;
    return newmap;
}

int treemap_mem(TreeMap *map, void *x, TreeMapCmpFunc cmp)
{
    if(treemap_is_empty(map)){
        return 0;
    }

    void *y = map->val;
    TreeMap *l = map->l;
    TreeMap *r = map->r;

    int cmp_result = cmp(x, y);
    if(cmp_result < 0){
        return treemap_mem(l, x, cmp);
    }
    else if(cmp_result > 0){
        return treemap_mem(r, x, cmp);
    }
    else{
        return 1;
    }
}

char *treemap_show(TreeMap *map, TreeMapShowFunc show)
{
    char *str = malloc(sizeof(char) * 100);

    if(treemap_is_empty(map)){
        sprintf(str, "Empty");
        return str;
    }

    char *color;
    if(treemap_is_black(map)){
        color = "Black";
    }
    else{
        color = "Red";
    }
    char *val = show(map->val);
    char *l = treemap_show(map->l, show);
    char *r = treemap_show(map->r, show);

    sprintf(str, "%s %s, (%s), (%s)", color, val, l, r);
    return str;
}


/*
int cmp_int(const void *a, const void *b)
{
    int x = *(int*)a;
    int y = *(int*)b;
    if(x < y){
        return -1;
    }
    else if(x > y){
        return 1;
    }
    else{
        return 0;
    }
}

int cmp_str(const void *a, const void *b)
{
    return strcmp((char *)a, (char *)b);
}

char *show_int(const void *a)
{
    char *str = malloc(sizeof(char) * 10);
    int x = *(int*)a;
    sprintf(str, "%d", x);
    return str;
}

char *show_str(const void *a)
{
    char *str = malloc(sizeof(char) * 10);
    sprintf(str, "%s", (char *)a);
    return str;
}

int main()
{
    int x1, x2, x3, x4, x5, z;
    TreeMap *tm1 = treemap_empty();

    x1 = 10;
    tm1 = treemap_insert(tm1, &x1, cmp_int);
    x2 = 20;
    tm1 = treemap_insert(tm1, &x2, cmp_int);
    x3 = 30;
    tm1 = treemap_insert(tm1, &x3, cmp_int);
    x4 = 40;
    tm1 = treemap_insert(tm1, &x4, cmp_int);
    x5 = 50;
    tm1 = treemap_insert(tm1, &x5, cmp_int);

    printf("%s\n", treemap_show(tm1, show_int));

    z = 30;
    printf("[tree-map has %d] is %d.\n", z, treemap_mem(tm1, &z, cmp_int));
    z = 70;
    printf("[tree-map has %d] is %d.\n", z, treemap_mem(tm1, &z, cmp_int));

    TreeMap *tm2 = treemap_empty();
    tm2 = treemap_insert(tm2, "o", cmp_str);
    tm2 = treemap_insert(tm2, "p", cmp_str);
    tm2 = treemap_insert(tm2, "q", cmp_str);
    tm2 = treemap_insert(tm2, "a", cmp_str);
    tm2 = treemap_insert(tm2, "z", cmp_str);
    tm2 = treemap_insert(tm2, "b", cmp_str);
    tm2 = treemap_insert(tm2, "x", cmp_str);
    tm2 = treemap_insert(tm2, "c", cmp_str);
    tm2 = treemap_insert(tm2, "y", cmp_str);

    printf("%s\n", treemap_show(tm2, show_str));
    printf("[tree-map has %s] is %d.\n", "z", treemap_mem(tm2, "z", cmp_str));
    printf("[tree-map has %s] is %d.\n", "Z", treemap_mem(tm2, "Z", cmp_str));

    return 0;
}
*/
