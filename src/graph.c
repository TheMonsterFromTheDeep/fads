#include <zlib/zlist.h>
#include <zlib/zstr.h>
#include <stdlib.h>

#include "graph.h"
#include "expression.h"
#include "equation.h"

static int currentcolor;

static int nextcolor() {
    currentcolor = (currentcolor + 1) % COLOR_GRAPH_COUNT;
    return currentcolor + GRAPH_RED;
}

graph graph_create() {
    return (graph){
        NULL,
        zstr_empty(),
        nextcolor()
    };
}

int graph_valid(graph g) {
    return !!g.expression;
}

void graph_setup() {
    zlist_init(graphs, 0);
}

void graph_update(size_t index) {
    graph *ptr = zlist_get_ptr(graphs, index);
    ptr->expression = eq_parse(ptr->equation->data);
}

void graph_freeall() {
    size_t i;
    for(i = 0; i < zlist_size(graphs); ++i) {
        expr_free(zlist_get(graphs, i).expression);
        zstr_free(zlist_get(graphs, i).equation);
    }
}
