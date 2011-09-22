#ifndef _map_h_
#define _map_h_

#include <stdlib.h>
#include "list.h"
#include "vector.h"

#define MAP_SLOT_NUM 32

typedef struct {
    const char *key;
    void *val;
} MapNode;

typedef struct {
    int n;
    List **slots;
} Map;


Map *map_new();
void map_destroy(Map *map);

void map_set(Map *map, const char *key, void *val);
void *map_get(Map *map, const char *key);
int map_has(Map *map, const char *key);
void map_remove(Map *map, const char *key);
Map *map_clone(Map *src);
int map_exists(Map *map, void *val);
Vector *map_keys(Map *map);
Vector *map_values(Map *map);


#endif
