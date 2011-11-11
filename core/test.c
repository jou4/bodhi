#include <stdio.h>
#include <stdlib.h>
#include "gc.h"
#include "value.h"
#include "core.h"

extern GC *gc;
extern Vector *globals;
extern void bodhi_print();
extern void *NIL_PTR;


int main()
{
	gc_init(1000, 1000);

	long a = 1, b = 2, c = 3;

	BDValue *v = bd_value_list((void *)a, NIL_PTR);
	bodhi_print(v);
	v = bd_value_list((long *)b, v);
	bodhi_print(v);
	v = bd_value_list((long *)c, v);
	bodhi_core_push_global_ptr(&v);

	printf("%p", v);
	bodhi_print(v);

	gc_start();

	printf("%p", v);
	bodhi_print(v);

	gc_start();

	printf("%p", v);
	bodhi_print(v);

	gc_dump(gc);

	gc_finish();

	return 0;
}
