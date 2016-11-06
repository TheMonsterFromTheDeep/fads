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
expr *expr_new_sec(expr*);
expr *expr_new_csc(expr*);
expr *expr_new_cot(expr*);
expr *expr_new_arcsin(expr*);
expr *expr_new_arccos(expr*);
expr *expr_new_arctan(expr*);
expr *expr_new_arcsec(expr*);
expr *expr_new_arccsc(expr*);
expr *expr_new_arccot(expr*);
expr *expr_new_floor(expr*);
expr *expr_new_ceil(expr*);
expr *expr_new_round(expr*);
expr *expr_new_abs(expr*);
expr *expr_new_ln(expr*);
expr *expr_new_sqrt(expr*);
expr *expr_new_range(expr*,expr*);
expr *expr_new_int(expr*,expr*);

void expr_free(expr*);

#endif
