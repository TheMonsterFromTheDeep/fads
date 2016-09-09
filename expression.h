#ifndef EXPRESSION_H
#define EXPRESSION_H

typedef float (*evaluator)(void *data, float x);

typedef struct expression {
    void* data;
    evaluator evaluate;
    void (*free)(void *data);
} expression_td;

typedef expression_td *expr;

expr newexpr(void);

float evalexpr(expr, float);

expr newconstexpr(float);
expr newxexpr();
expr newaddexpr(expr, expr);
expr newsubexpr(expr, expr);
expr newmulexpr(expr, expr);
expr newdivexpr(expr, expr);
expr newpowexpr(expr, expr);

//float evalconst(void*, float);
//float evalx(void*, float);
//float evaladd(void*, float);
//float evalsub(void*, float);
//float evalmul(void*, float);
//float evaldiv(void*, float);

void freeexpr(expr);

#endif
