#ifndef GRAPH_H
#define GRAPH_H

#include <zlib/zlist.h>
#include <zlib/zstr.h>
#include <stdlib.h>
#include "expression.h"

typedef struct graph {
    expr *expression;
    zstr *equation;
    int color;
} graph;

zlist_of(graph) graphs;

graph graph_create(void);
int graph_valid(graph);

void graph_update(size_t);

void graph_setup(void);
void graph_freeall(void);

#endif
