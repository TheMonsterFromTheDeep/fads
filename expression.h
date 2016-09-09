#ifndef EXPRESSION_H
#define EXPRESSION_H

typedef float (*evaluator)(void *data, float x);

typedef struct expr {
    void* data;
    evaluator evaluate;
    void (*free)(void *data);
} expr;

expr *expr_new(void);

float expr_eval(expr*, float);

expr *expr_new_const(float);
expr *expr_new_x();
expr *expr_new_add(expr*, expr*);
expr *expr_new_sub(expr*, expr*);
expr *expr_new_mul(expr*, expr*);
expr *expr_new_div(expr*, expr*);
expr *expr_new_pow(expr*, expr*);

void expr_free(expr*);

#endif
