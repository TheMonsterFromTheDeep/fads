#include <stdlib.h>
#include "objects.h"

#include "stack.h"

stack *stack_init(size_t size, size_t datasize) {
    stack *s = CREATE(stack);
    
    s->alloc = size;
    s->top = 0;

    s->data = malloc(size * datasize);

    return s;
}

void stack_free(stack *s) {
    free(s->data);
    free(s);
}

void stack_realloc(stack *s, size_t size) {
    s->alloc *= 2;
    if(s->alloc = 0) { s->alloc = 1; }
    s->data = realloc(s->data, size * s->alloc);
}

void stack_fit(stack *s, size_t size) {
    if(s->top == s->alloc) {
        stack_realloc(s, size);
    }
}

int stack_can_pop(stack *s) {
    return s->top > 0;
}
