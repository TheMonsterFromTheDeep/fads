#ifndef GRAPH_H
#define GRAPH_H

#include "base.h"
#include "expression.h"

typedef enum graphtype {
    SINGLE,
    MULTI
} graphtype;

typedef struct graph {
    expr *expression;
    graphtype type;
    int color;
} graph;

void grf_init(void);

void grf_draw(void);
void grf_loop(void);

void grf_scale(int, int);
void grf_scaleby(int, int);
void grf_pan(num, num);
void grf_panby(num, num);
void grf_center(void);

void grf_end(void);

size_t grf_graphcount();
graph grf_getgraph(size_t);
graph grf_addgraph(expr*);
void grf_removegraph(size_t);

const int grf_isinvalid(graph g);

#endif
