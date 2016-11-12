#ifndef GRAPH_H
#define GRAPH_H

#include <zlib/zlist.h>
#include "expression.h"

typedef struct graph {
    expr *expression;
    int color;
} graph;

zlist_of(graph) graphs;

graph graph_create(expr*);
int graph_valid(graph);

#endif
