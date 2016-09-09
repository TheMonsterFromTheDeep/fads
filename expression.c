#include <stdlib.h>
#include <math.h>

#include "expression.h"

void freenull(void *data) { }

void freedouble(void *data) {
    expr* exprs = (expr*)data;
    freeexpr(exprs[0]);
    freeexpr(exprs[1]);
}

float evalexpr(expr e, float x) {
    return e->evaluate(e->data, x);
}

expr newexpr() {
    return malloc(sizeof(expression_td));
}

expr newdoubleexpr(expr first, expr second, evaluator ev) {
    expr e = newexpr();
    e->evaluate = ev;
    e->data = malloc(sizeof(expr) * 2);
    ((expr*)e->data)[0] = first;
    ((expr*)e->data)[1] = second;
    e->free = &freedouble;
    return e;
}

float evalconst(void *data, float x) {
    return ((float*)data)[0];
}

expr newconstexpr(float val) {
    expr e = newexpr();
    e->evaluate = &evalconst;
    e->data = malloc(sizeof(float));
    *((float*)e->data) = val;
    e->free = &freenull;
    return e;
}

float evalx(void *data, float x) {
    return x;
}

expr newxexpr() {
    expr e = newexpr();
    e->evaluate = &evalx;
    e->free = &freenull;
    return e;
}

float evaladd(void *data, float x) {
    expr* exprs = (expr*)data;
    return evalexpr(exprs[0],x) + evalexpr(exprs[1],x);
}

expr newaddexpr(expr first, expr second) {
    return newdoubleexpr(first, second, &evaladd);
}

float evalsub(void *data, float x) {
    expr* exprs = (expr*)data;
    return evalexpr(exprs[0],x) - evalexpr(exprs[1],x);
}

expr newsubexpr(expr first, expr second) {
    return newdoubleexpr(first, second, &evalsub);
}

float evalmul(void *data, float x) {
    expr* exprs = (expr*)data;
    return evalexpr(exprs[0],x) * evalexpr(exprs[1],x);
}

expr newmulexpr(expr first, expr second) {
    return newdoubleexpr(first, second, &evalmul);
}

float evaldiv(void *data, float x) {
    expr* exprs = (expr*)data;
    return evalexpr(exprs[0],x) / evalexpr(exprs[1],x);
}

expr newdivexpr(expr first, expr second) {
    return newdoubleexpr(first, second, &evaldiv);
}

float evalpow(void *data, float x) {
    expr* exprs = (expr*)data;
    return powf(evalexpr(exprs[0],x), evalexpr(exprs[1],x));
}

expr newpowexpr(expr first, expr second) {
    return newdoubleexpr(first, second, &evalpow);
}

void freeexpr(expr e) {
    e->free(e->data);
    free(e->data);
    free(e);
}
