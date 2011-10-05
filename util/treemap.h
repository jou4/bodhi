#ifndef _treemap_h_
#define _treemap_h_


typedef enum { TM_BLACK, TM_RED } TreeMapColor;
typedef struct TreeMap TreeMap;
typedef int(*TreeMapCmpFunc)(const void *, const void *);
typedef char *(*TreeMapShowFunc)(const void *);

struct TreeMap {
    TreeMapColor color;
    void *val;
    TreeMap *l;
    TreeMap *r;
};

TreeMap *treemap_new(TreeMapColor color, void *val, TreeMap *l, TreeMap *r);
TreeMap *treemap_empty();
int treemap_is_empty(TreeMap *map);
int treemap_is_black(TreeMap *map);
int treemap_is_red(TreeMap *map);
TreeMap *treemap_insert(TreeMap *map, void *x, TreeMapCmpFunc cmp);
int treemap_mem(TreeMap *map, void *x, TreeMapCmpFunc cmp);
char *treemap_show(TreeMap *map, TreeMapShowFunc show);


#endif
