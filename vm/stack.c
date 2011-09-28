#include "stack.h"

Stack *stack_create()
{
    return vector_new();
}

void stack_destory(Stack *stack)
{
    vector_destroy(stack);
}

void stack_init(Stack *stack)
{
    stack->length = 0;
}

void stack_push(Stack *stack, Value *value)
{
    vector_add(stack, value);
}

Value *stack_pop(Stack *stack)
{
    int end = stack->length - 1;
    Value *val = vector_get(stack, end);
    vector_set(stack, end, NULL);
    stack->length = end;
    return val;
}

Value *stack_get(Stack *stack, int index)
{
    return (Value *)vector_get(stack, index);
}

StackFrame *stack_frame_create(StackFrame *prev)
{
    StackFrame *frame = malloc(sizeof(StackFrame));
    frame->prev = prev;
    frame->next = NULL;
    frame->stack = stack_create();
    return frame;
}

void stack_frame_destroy(StackFrame *frame)
{
    stack_destroy(frame->stack);
    free(frame);
}

StackFrameList *stack_frame_list_create()
{
    StackFrameList *list = malloc(sizeof(StackFrameList));
    list->top = stack_frame_create(NULL);
    list->current = list->top;
    return list;
}

void stack_frame_list_destroy(StackFrameList *list)
{
    StackFrame *frame = list->top;
    StackFrame *next;

    while(frame){
        next = frame->next;
        stack_frame_destroy(frame);
        frame = next;
    }

    free(list);
}

void stack_frame_add(StackFrameList *list)
{
    StackFrame *frame = stack_frame_create(list->current);
    list->current->next = frame;
    list->current = frame;
}

void stack_frame_remove(StackFrameList *list)
{
    StackFrame *frame = list->current;
    list->current = frame->prev;
    stack_frame_destroy(frame);
}
