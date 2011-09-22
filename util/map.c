#include <stdio.h>
#include <string.h>
#include "map.h"

MapNode *map_node_new(const char *key, void *val)
{
    MapNode *node = (MapNode *)malloc(sizeof(MapNode));
    node->key = key;
    node->val = val;
    return node;
}

void map_node_destroy(MapNode *node)
{
    free(node);
}

Map *map_new()
{
    Map *map = (Map *)malloc(sizeof(Map));
    map->n = MAP_SLOT_NUM;
    map->slots = (List **)malloc(sizeof(List) * map->n);

    int i;
    for(i = 0; i < map->n; i++){
        map->slots[i] = NULL;
    }

    return map;
}

void map_destroy(Map *map)
{
    int i;
    List *slot;
    MapNode *node;

    for(i = 0; i < map->n; i++){
        slot = map->slots[i];
        while(slot){
            map_node_destroy(list_head(slot));
            slot = list_tail(slot);
        }
        list_destroy(map->slots[i]);
    }

    free(map->slots);
    free(map);
}

int gen_hash(const char *key, int n)
{
    int sum = 0;
    char c;
    int i = 0;
    while((c = key[i++]) != '\0'){
        sum += (int)c;
    }
    return sum % n;
}

MapNode *map_search(Map *map, const char *key)
{
    int slot_index = gen_hash(key, map->n);
    List *slot = map->slots[slot_index];

    MapNode *node;
    while(slot){
        node = list_head(slot);
        if(node != NULL && strcmp(node->key, key) == 0){
            return node;
        }
        slot = list_tail(slot);
    }

    return NULL;
}

void map_set(Map *map, const char *key, void *val)
{
    MapNode *node = map_search(map, key);
    if(node != NULL){
        node->val = val;
        return;
    }

    node = map_node_new(key, val);

    int slot_index = gen_hash(key, map->n);
    map->slots[slot_index] = list_cons(node, map->slots[slot_index]);
}

void *map_get(Map *map, const char *key)
{
    MapNode *node = map_search(map, key);
    if(node != NULL){
        return node->val;
    }
    return NULL;
}

int map_has(Map *map, const char *key)
{
    MapNode *node = map_search(map, key);
    if(node != NULL){
        return 1;
    }
    return 0;
}

void map_remove(Map *map, const char *key)
{
    int slot_index;
    List *slot, *prev;
    MapNode *node;

    slot_index = gen_hash(key, map->n);
    slot = map->slots[slot_index];
    prev = NULL;

    while(slot){
        node = list_head(slot);
        if(node != NULL && strcmp(node->key, key) == 0){
            map_node_destroy(node);

            if(prev){
                list_tail(prev) = list_tail(slot);
            }else{
                map->slots[slot_index] = list_tail(slot);
            }

            list_tail(slot) = NULL;
            list_destroy(slot);
            break;
        }
        prev = slot;
        slot = list_tail(slot);
    }
}

Map *map_clone(Map *src)
{
    Map *map = map_new();

    int i;
    List *slot;
    MapNode *new_node, *src_node;
    for(i = 0; i < map->n; i++){
        slot = src->slots[i];
        while(slot)
        {
            src_node = list_head(slot);
            new_node = map_node_new(src_node->key, src_node->val);
            map->slots[i] = list_cons(new_node, map->slots[i]);
            slot = list_tail(slot);
        }
    }

    return map;
}

int map_exists(Map *map, void *val)
{
    int i, index;
    List *slot;
    MapNode *node;

    for(i = 0; i < map->n; i++){
        slot = map->slots[i];
        while(slot)
        {
            node = list_head(slot);
            if(node->val == val){
                return 1;
            }
            slot = list_tail(slot);
        }
    }
    return 0;
}

Vector *map_keys(Map *map)
{
    int i;
    List *slot;
    MapNode *node;
    Vector *keys = vector_new();

    for(i = 0; i < map->n; i++){
        slot = map->slots[i];
        while(slot)
        {
            node = list_head(slot);
            vector_add(keys, (void *)node->key);
            slot = list_tail(slot);
        }
    }

    return keys;
}

Vector *map_values(Map *map)
{
    int i;
    List *slot;
    MapNode *node;
    Vector *vals = vector_new();

    for(i = 0; i < map->n; i++){
        slot = map->slots[i];
        while(slot)
        {
            node = list_head(slot);
            vector_add(vals, node->val);
            slot = list_tail(slot);
        }
    }

    return vals;
}


/*
void map_test(Map *map, const char *key)
{
    void *val = map_get(map, key);
    if(val != NULL){
        printf("%s=%s\n", key, (char *)val);
    }
    else{
        printf("%s=%s\n", key, "(NULL)");
    }
}

int main()
{
    char *str = (char *)malloc(sizeof(char) * 2);
    str[0] = 'z';
    str[1] = '\0';

    Map *map = map_new();
    map_set(map, "AAA", "111");
    map_set(map, "BBB", "222");
    map_set(map, "CCC", "333");
    map_set(map, "AAA", "000");

    Map *clone = map_clone(map);
    map_set(clone, "AAA", "***");
    map_set(clone, "DDD", "444");

    map_set(map, "EEE", "555");
    map_set(map, "ZZZ", str);

    printf("original\n");
    map_test(map, "AAA");
    map_test(map, "BBB");
    map_test(map, "CCC");
    map_test(map, "DDD");
    map_test(map, "EEE");

    printf("clone\n");
    map_test(clone, "AAA");
    map_test(clone, "BBB");
    map_test(clone, "CCC");
    map_test(clone, "DDD");
    map_test(clone, "EEE");

    map_remove(map, "AAA");

    printf("original\n");
    map_test(map, "AAA");
    map_test(map, "BBB");
    map_test(map, "CCC");
    map_test(map, "DDD");
    map_test(map, "EEE");

    printf("clone\n");
    map_test(clone, "AAA");
    map_test(clone, "BBB");
    map_test(clone, "CCC");
    map_test(clone, "DDD");
    map_test(clone, "EEE");

    printf("map has 'str' : %d\n", map_exists(map, str));
    printf("clone has 'str' : %d\n", map_exists(clone, str));

    int i;
    Vector *keys = map_keys(map);
    for(i = 0; i < keys->length; i++){
        printf("%s\n", (char *)vector_get(keys, i));
    }

    Vector *vals = map_values(map);
    for(i = 0; i < vals->length; i++){
        printf("%s\n", (char *)vector_get(vals, i));
    }

    map_destroy(map);
    map_destroy(clone);

    return 0;
}
*/
