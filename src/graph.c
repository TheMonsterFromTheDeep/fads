#include <zlib/zlist.h>

#include "graph.h"
#include "expression.h"

static int currentcolor;

static int nextcolor() {
    currentcolor = (currentcolor + 1) % COLOR_GRAPH_COUNT;
    return currentcolor + GRAPH_RED;
}

graph graph_create(expr *e) {
    return (graph){
        e,
        nextcolor()
    };
}

int graph_valid(graph g) {
    return !!g.expression;
}

void graph_setup() {
    zlist_init(graphs, 0);
}

void graph_freeall() {
    size_t i;
    for(i = 0; i < zlist_size(graphs); ++i) {
        expr_free(zlist_get(graphs, i).expression);
    }
}
