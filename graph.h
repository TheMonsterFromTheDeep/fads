#ifndef GRAPH_H
#define GRAPH_H

#include "expression.h"

typedef struct graph {
    expr expression;
    int color;
} graph;

void grf_init(void);

void grf_draw(void);
int grf_loop(void);

void grf_scale(int, int);
void grf_scaleby(int, int);
void grf_pan(float, float);
void grf_panby(float, float);

void grf_end(void);

#endif
