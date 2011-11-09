#include <stdio.h>
#include <stdlib.h>

typedef struct {
    char *a;
    char *b;
} Value;

int main()
{
    char *a = "ABC";
    char *b = "DEF";

    Value *v = malloc(sizeof(Value));
    v->a = a;
    v->b = b;

    long *v2 = malloc(sizeof(Value));
    v2[0] = (long)a;
    v2[1] = (long)b;

    printf("%s\n", ((Value *)v)->a);
    printf("%s\n", ((Value *)v)->b);
    printf("%s\n", ((Value *)v2)->a);
    printf("%s\n", ((Value *)v2)->b);

    return 0;
}
