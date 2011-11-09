#include <stdio.h>
#include <stdlib.h>

typedef struct {
    char *entry;
} Closure;

void *_make_closure(char *entry, int size)
{
    Closure *value = malloc(sizeof(void *) + size);
    value->entry = entry;
    return value;
}

void *make_closure(char *entry, int size)
{
    long *value = malloc(sizeof(void *) + size);
    value[0] = (long)entry;
    return value;
}

char *entry(void *value)
{
    return ((Closure*)value)->entry;
}

void *freevars(void *value)
{
    return value + sizeof(void *);
}

void *new_closure(char *entry, int fvs_size, char init)
{
    void *cls = make_closure(entry, fvs_size);

    char *fvs = cls + sizeof(void *);
    int i;
    for(i = 0; i < fvs_size - 1; i++){
        fvs[i] = init;
    }
    fvs[i] = '\0';

    return cls;
}


int main()
{
    void *cls1 = new_closure("ABC", 24, 'A');
    void *cls2 = new_closure("DEF", 16, 'B');
    void *cls3 = new_closure("GHI", 32, 'C');

    printf("%s\n", (char *)entry(cls1));
    printf("%s\n", (char *)freevars(cls1));

    printf("%s\n", (char *)entry(cls2));
    printf("%s\n", (char *)freevars(cls2));

    printf("%s\n", (char *)entry(cls3));
    printf("%s\n", (char *)freevars(cls3));

    free(cls1);
    free(cls2);
    free(cls3);

    return 0;
}
