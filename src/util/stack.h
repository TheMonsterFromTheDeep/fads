#ifndef UTIL_STACK_H
#define UTIL_STACK_H

#include <stdlib.h>

typedef struct stack {
    size_t top;
    size_t alloc;
    void *data;
} stack;

#define stack_new(type,size) (stack_init(size, sizeof(type)))
#define stack_push(s,t,e) stack_fit(s,sizeof(t));((t*)s->data)[s->top++]=e
#define stack_pop(s,t) (((t*)s->data)[s->top>0?--s->top:0])
#define stack_read(s,t,i) (((t*)s->data)[i])

stack *stack_init(size_t,size_t);
void stack_free(stack*);

void stack_realloc(stack*,size_t);
void stack_fit(stack*,size_t);

int stack_can_pop(stack*);

#define stack_generate(name,type) \
    stack *name; \
    void name ## _init(size) { name = stack_new(type,size); } \
    void name ## _free() { stack_free(name); } \
    void name ## _push(type n) { stack_push(name,type,n); } \
    type name ## _pop() { return stack_pop(name,type); } \
    int name ## _can_pop() { return stack_can_pop(name); } \
    type name ## _read(size_t index) { return stack_read(name, type, index); }

#endif
