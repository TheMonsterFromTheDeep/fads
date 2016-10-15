#include <stdlib.h>
#include <ncurses.h>
#include <math.h>

#include "base.h"
#include "graph.h"
#include "expression.h"
#include "brscr.h"

static graph *graphs; /* TODO : Use some list library or something */
static size_t graphcount;
static size_t graphalloc;

static int currentcolor;

static float panx, pany;
static float sclx, scly;

static int sclex, scley;

const graph err = { NULL, 0 };

static brscr *graphscr;

static int nextcolor() {
    if(currentcolor < COLOR_GRAPH_COUNT) { ++currentcolor; }
    else { currentcolor = 0; }
    return currentcolor;
}

static int getcolorcode(int color) {
    switch(color) {
        case 0:
            return GRAPH_RED;
            break;
        case 1:
            return GRAPH_GREEN;
            break;
        case 2:
        default: /* Failsafe - it would be silly for the program
                     to break completely simply due to poor color programming */
            return GRAPH_BLUE;
            break;
    }
}

static float trx(num x) {
    return sclx * (x - panx);
}

static float itrx(num x) {
    return (x / sclx) + panx;
}

static float try(num y) {
    return pany - (scly * y);
}

static float itry(num y) {
    return (pany - y);
}

static void swap(num *a, num *b) {
    num tmp = *a;
    *a = *b;
    *b = tmp;
}

static float plot(expr *e, float x) {
    return try(expr_eval(e, trx(x)));
}

void grf_draw() {
    int x, y, i;
    int x0 = (int)roundf(itrx(0)), y0 = (int)roundf(itry(0));
    int *color = malloc(sizeof(int) * graphscr->size);
    br_clear(graphscr);
    clear();

    attron(COLOR_PAIR(GRAPH_AXES));

    for(i = 0; i < graphcount; ++i) {
        for(x = 0; x < br_width(graphscr); ++x) {
            br_setstate(graphscr, x, (int)roundf(plot(graphs[i].expression, x)), 1);
        }
    }

    br_drawtocurse(graphscr);

    /*for(y = 0; y < screen.height; ++y) { /* Fill screen and draw axes 
        move(y, 0);
        for(x = 0; x < screen.width; ++x) {
            addch(x == x0 ? (y == y0 ? ACS_PLUS : ACS_VLINE) : y == y0 ? ACS_HLINE : ' ');
        }
    }*/

    free(color); /* Maybe this should be global? */
    
    refresh();	
}

static void updatescale() {
    sclx = pow(1.1, -sclex); /* Invert sclx so +scale -> zooming in */
    scly = pow(1.1, scley); /* scly needs to be the inverse of sclx */
    
    grf_draw(); /* Refresh graph screen */
}

static graph newgraph(expr *e) {
    graph g = {
        e,
        SINGLE,
        nextcolor()
    };
    return g;
}

size_t grf_graphcount() {
    return graphcount;
}

graph grf_getgraph(size_t index) {
    if(index >= graphcount) { return err; }
    return graphs[index];
}

graph grf_addgraph(expr *e) {
    graph ng = newgraph(e);
    
    if(graphcount == graphalloc) {
        graphalloc = graphalloc * 2 + 1;
        graphs = realloc(graphs, sizeof(graph) * (graphalloc));
    }

    graphs[graphcount++] = ng;
    
    return ng;
}

void grf_removegraph(size_t index) {
    if(index >= graphcount) { return; }
    expr_free(graphs[index].expression);
    --graphcount;
    size_t i;
    /* TODO: Make graph pointer type? */
    for(i = index; i < graphcount; ++i) { /* Graphcount has been decreased, but there is still a valid graph there */
        graphs[i] = graphs[i + 1];
    }
}

int grf_invalid(graph g) {
    return !g.expression;
}

void grf_init() {
    graphs = NULL;
    graphcount = 0;
    graphalloc = 0;
    grf_addgraph(expr_new_pow(expr_new_x(), expr_new_const(2)));

    init_pair(GRAPH_AXES, COLOR_BLACK, COLOR_WHITE);
    init_pair(GRAPH_RED, COLOR_RED, COLOR_WHITE);
    init_pair(GRAPH_GREEN, COLOR_GREEN, COLOR_WHITE);
    init_pair(GRAPH_BLUE, COLOR_BLUE, COLOR_WHITE);

    graphscr = br_scrfromcurse();

    panx = screen.width / 2.0f; /* Make sure to divide by floats */
	pany = screen.height / 2.0f;
	
	sclex = scley = 4;
	updatescale(); /* Initial draw call as well */
}

void grf_scale(int x, int y) {
    sclex = x;
    scley = y;
    updatescale();
}

void grf_scaleby(int dx, int dy) {
    sclex += dx;
    scley += dy;
    updatescale();
}

void grf_pan(num x, num y) {
    panx = x;
    pany = y;
    grf_draw();
}

void grf_panby(num dx, num dy) {
    panx += dx;
    pany += dy;
    grf_draw();
}

void grf_loop() {
    int ch = getch();
    switch(ch) {
        case 'e':
            grf_scaleby(1, 1);
            break;
        case 'r':
            grf_scaleby(-1, -1);
            break;
        case 'w':
            grf_panby(0, 1);
            break;
        case 's':
            grf_panby(0, -1);
            break;
        case 'a':
            grf_panby(1, 0);
            break;
        case 'd':
            grf_panby(-1, 0);
            break;
        case 'q':
            quit(0);
            break;
        case '.': /* This will be reimplemented soon... */
        case '>':
            mode_set(TERMINAL);
    }
}

void grf_end() {
    int i;
    for(i = 0; i < graphcount; ++i) {
        expr_free(graphs[i].expression);
    }
    free(graphs);
    br_free(graphscr);
}
