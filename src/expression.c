#include <stdlib.h>
#include <math.h>

#include "base.h"
#include "expression.h"

/* TODO: Come up with better macro names */

#define EXPR_EVAL_N(n) (expr_eval(((expr**)data)[n], x))
#define EXPR_EVAL_N_AT(n,v) (expr_eval(((expr**)data)[n], v))
#define EXPR_EVAL_D(op) (EXPR_EVAL_N(0) op EXPR_EVAL_N(1))
#define EXPR_EVAL_D_FUNC(f) (f(EXPR_EVAL_N(0), EXPR_EVAL_N(1)))
#define EXPR_EVAL_S_FUNC(f) (f(EXPR_EVAL_N(0)))

static num evalconst(void *data, num x) {
    return ((float*)data)[0];
}

static num evalx(void *data, num x) {
    return x;
}

static num evaladd(void *data, num x) {
    return EXPR_EVAL_D(+);
}

static num evalsub(void *data, num x) {
    return EXPR_EVAL_D(-);
}

static num evalmul(void *data, num x) {
    return EXPR_EVAL_D(*);
}

static num evaldiv(void *data, num x) {
    return EXPR_EVAL_D(/);
}

static num evalpow(void *data, num x) {
    return EXPR_EVAL_D_FUNC(powf);
}

static num evalsin(void *data, num x) {
    return EXPR_EVAL_S_FUNC(sin);
}

static num evalcos(void *data, num x) {
    return EXPR_EVAL_S_FUNC(cos);
}

static num evaltan(void *data, num x) {
    return EXPR_EVAL_S_FUNC(tan);
}

static num evalrange(void *data, num x) {
    if(x <= 0) { return EXPR_EVAL_N_AT(0,0); }
    if(x >= 1) { return EXPR_EVAL_N_AT(1,0); }
    return (EXPR_EVAL_N_AT(1,0) - EXPR_EVAL_N_AT(0,0)) * x + EXPR_EVAL_N_AT(0,0);
}

static num evalint(void *data, num x) {
    static const int n = 100;

    int i = 0;

    num left = EXPR_EVAL_N_AT(0,0);
    num right = EXPR_EVAL_N_AT(0,1);

    if(((expr**)data)[0]->evaluate == &evalrange) {
        left = expr_eval( ((expr**)(((expr**)data)[0])->data)[0], x);
        right = expr_eval( ((expr**)(((expr**)data)[0])->data)[1], x);
    }

    num step = (right - left) / n;
    
    float result = 0;
    for(i = 1; i <= n - 1; i += 2) {
        result += EXPR_EVAL_N_AT(1,left + i * step);
    }
    result *= 2;
    for(i = 2; i <= n - 2; i += 2) {
        result += EXPR_EVAL_N_AT(1,left + i * step);
    }
    result *= 2;
    result += EXPR_EVAL_N_AT(1,left) + EXPR_EVAL_N_AT(1,right);

    result *= (right - left) / (3 * n);

    return result;
}

#undef EXPR_EVAL_N
#undef EXPR_EVAL_N_AT
#undef EXPR_EVAL_D
#undef EXPR_EVAL_D_FUNC
#undef EXPR_EVAL_S_FUNC

/*-----------------------------------------------------------*/

static void freenull(void *data) { }

static void freesingle(void *data) {
    expr_free(*((expr**)data));
}

static void freedouble(void *data) {
    expr **exprs = (expr**)data;
    expr_free(exprs[0]);
    expr_free(exprs[1]);
}

static expr *expr_new_single(expr *inner, evaluator ev) {
    expr *e = expr_new();
    e->evaluate = ev;
    e->data = malloc(sizeof(expr));
    *((expr**)e->data) = inner;
    e->free = &freesingle;
    return e;
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

num expr_eval(expr *e, num x) {
    return e->evaluate(e->data, x);
}

void expr_free(expr *e) {
    e->free(e->data);
    free(e->data);
    free(e);
}

expr *expr_new_const(num val) {
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

expr *expr_new_sin(expr *inner) {
    return expr_new_single(inner, &evalsin);
}

expr *expr_new_cos(expr *inner) {
    return expr_new_single(inner, &evalcos);
}

expr *expr_new_tan(expr *inner) {
    return expr_new_single(inner, &evaltan);
}

expr *expr_new_range(expr* first, expr *second) {
    return expr_new_double(first, second, &evalrange);
}

expr *expr_new_int(expr *range, expr *expression) {
    return expr_new_double(range, expression, &evalint);
}
