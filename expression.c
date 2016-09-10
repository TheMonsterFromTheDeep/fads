#include <stdlib.h>
#include <math.h>

#include "expression.h"

/* TODO: Come up with better macro names */

#define EXPR_EVAL_N(n) (expr_eval(((expr**)data)[n], x))
#define EXPR_EVAL_D(op) (EXPR_EVAL_N(0) op EXPR_EVAL_N(1))
#define EXPR_EVAL_D_FUNC(f) (f(EXPR_EVAL_N(0), EXPR_EVAL_N(1)))

static float evalconst(void *data, float x) {
    return ((float*)data)[0];
}

static float evalx(void *data, float x) {
    return x;
}

static float evaladd(void *data, float x) {
    return EXPR_EVAL_D(+);
}

static float evalsub(void *data, float x) {
    return EXPR_EVAL_D(-);
}

static float evalmul(void *data, float x) {
    return EXPR_EVAL_D(*);
}

static float evaldiv(void *data, float x) {
    return EXPR_EVAL_D(/);
}

static float evalpow(void *data, float x) {
    return EXPR_EVAL_D_FUNC(powf);
}

#undef EXPR_EVAL_N
#undef EXPR_EVAL_D
#undef EXPR_EVAL_D_FUNC

/*-----------------------------------------------------------*/

static void freenull(void *data) { }

static void freedouble(void *data) {
    expr **exprs = (expr**)data;
    expr_free(exprs[0]);
    expr_free(exprs[1]);
}

static expr *expr_new_double(expr *first, expr *second, evaluator ev) {
    expr *e = expr_new();
    e->evaluate = ev;
    e->data = malloc(sizeof(expr) * 2);
    ((expr**)e->data)[0] = first;
    ((expr**)e->data)[1] = second;
    e->free = &freedouble;
    return e;
}

expr *expr_new() {
    return malloc(sizeof(expr));
}

float expr_eval(expr *e, float x) {
    return e->evaluate(e->data, x);
}

void expr_free(expr *e) {
    e->free(e->data);
    free(e->data);
    free(e);
}

expr *expr_new_const(float val) {
    expr *e = expr_new();
    e->evaluate = &evalconst;
    e->data = malloc(sizeof(float));
    *((float*)e->data) = val;
    e->free = &freenull;
    return e;
}

expr *expr_new_x() {
    expr *e = expr_new();
    e->evaluate = &evalx;
    e->free = &freenull;
    return e;
}

expr *expr_new_add(expr *first, expr *second) {
    return expr_new_double(first, second, &evaladd);
}

expr *expr_new_sub(expr *first, expr *second) {
    return expr_new_double(first, second, &evalsub);
}

expr *expr_new_mul(expr *first, expr *second) {
    return expr_new_double(first, second, &evalmul);
}

expr *expr_new_div(expr *first, expr *second) {
    return expr_new_double(first, second, &evaldiv);
}

expr *expr_new_pow(expr *first, expr *second) {
    return expr_new_double(first, second, &evalpow);
}
