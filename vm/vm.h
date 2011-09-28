#ifndef _vm_h_
#define _vm_h_


#include "object.h"
#include "heap.h"
#include "stack.h"
#include "gc.h"

typedef struct {
    Heap *active;
    Heap *deactive;
    StackFrameList *frames;
    Stack *args;
    Value *result;
} VM;


VM *vm_create(Heap *active, Heap *deactive);
void vm_destroy(VM *vm);
void vm_gc(VM *vm);
void vm_add_var(VM *vm, Value *value);
void vm_push_arg(VM *vm, Value *value);
Value *vm_pop_arg(VM *vm);
Value *vm_get_result(VM *vm);


#endif
