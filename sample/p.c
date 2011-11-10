#include <stdio.h>
#include <stdlib.h>

typedef struct {
    void *a;
    void *b;
} Value;

char size()
{
    return sizeof(Value);
}

Value *create(void *a, void *b)
{
    Value *v = malloc(sizeof(Value));
    v->a = a;
    v->b = b;
    return v;
}

void *geta(Value *v)
{
    return v->a;
}

void *getb(Value *v)
{
    return v->b;
}

void show(Value *v)
{
    printf("%ld\n", (long)geta(v));
    printf("%ld\n", (long)getb(v));
}

int main()
{
    char *a = "ABC";
    char *b = "DEF";

    asm(
            "movq $100, %rdi\n"
            "movq $200, %rsi\n"
            "call _create\n"
            "movq %rax, %rdi\n"
            "call _show\n"
       );

    printf("%d\n", size());

    return 0;
}
