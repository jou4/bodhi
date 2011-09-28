#ifndef _stack_h_
#define _stack_h_

#include "../util/vector.h"
#include "object.h"

typedef Vector Stack;

typedef struct StackFrame StackFrame;

struct StackFrame {
    StackFrame *prev;
    StackFrame *next;
    Stack *stack;
};

typedef struct {
    StackFrame *top;
    StackFrame *current;
} StackFrameList;

Stack *stack_create();
void stack_destory(Stack *stack);
void stack_init(Stack *stack);
void stack_push(Stack *stack, Value *value);
Value *stack_pop(Stack *stack);
Value *stack_get(Stack *stack, int index);

StackFrame *stack_frame_create(StackFrame *prev);
void stack_frame_destroy(StackFrame *frame);

StackFrameList *stack_frame_list_create();
void stack_frame_list_destroy(StackFrameList *list);

void stack_frame_add(StackFrameList *list);
void stack_frame_remove(StackFrameList *list);


#endif
