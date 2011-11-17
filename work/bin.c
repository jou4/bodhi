#include <stdio.h>
#include <stdlib.h>

char *filename = "binfile";

typedef struct {
    int id;
    int price;
    int num;
} Order;

void print(int id, int price, int num)
{
    printf("%d: %d * %d\n", id, price, num);
}

void print_order(Order *o)
{
    print(o->id, o->price, o->num);
}

Order *new_order(int id, int price, int num)
{
    Order order = {id, price, num};
    Order *o = malloc(sizeof(Order));
    *o = order;
    return o;
}

int main()
{
    FILE *fp;
    Order *o;

    fp = fopen(filename, "wb");

    o = new_order(1, 100, 3);
    print_order(o);
    fwrite(o, sizeof(Order), 1, fp);
    free(o);

    o = new_order(2, 50, 5);
    print_order(o);
    fwrite(o, sizeof(Order), 1, fp);
    free(o);

    fclose(fp);

    Order o2;
    fp = fopen(filename, "rb");

    fread(&o2, sizeof(Order), 1, fp);
    print_order(&o2);

    fread(&o2, sizeof(Order), 1, fp);
    print_order(&o2);

    fclose(fp);

    fp = fopen(filename, "rb");
    int id, price, num;

    fread(&id, sizeof(int), 1, fp);
    fread(&price, sizeof(int), 1, fp);
    fread(&num, sizeof(int), 1, fp);
    print(id, price, num);

    fread(&id, sizeof(int), 1, fp);
    fread(&price, sizeof(int), 1, fp);
    fread(&num, sizeof(int), 1, fp);
    print(id, price, num);

    fclose(fp);


    void *o3 = new_order(10, 1000, 2);
    id = *(int*)(o3 + 0);
    price = *(int*)(o3 + 4);
    num = *(int*)(o3 + 8);
    print(id, price, num);
    free(o3);

    return 0;
}
