#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "base.h"

typedef num (*evaluator)(void *data, num x);

typedef struct expr {
    void* data;
    evaluator evaluate;
    void (*free)(void *data);
} expr;

expr *expr_new(void);

num expr_eval(expr*, num);

expr *expr_new_const(num);
expr *expr_new_x();
expr *expr_new_add(expr*, expr*);
expr *expr_new_sub(expr*, expr*);
expr *expr_new_mul(expr*, expr*);
expr *expr_new_div(expr*, expr*);
expr *expr_new_pow(expr*, expr*);
expr *expr_new_sin(expr*);
expr *expr_new_cos(expr*);
expr *expr_new_tan(expr*);

void expr_free(expr*);

#endif
