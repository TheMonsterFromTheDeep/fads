#include <stdlib.h>
#include <ncurses.h>
#include <math.h>

#include "base.h"
#include "graph.h"
#include "expression.h"
#include "brscr.h"
#include "config.h"
#include "colordefaults.h"

#define W_HLINE 0x2501
#define W_VLINE 0x2503
#define W_PLUS 0x254B

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
    currentcolor = (currentcolor + 1) % COLOR_GRAPH_COUNT;
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
    if(e == NULL) { return 0; } //TODO: Better error handling
    return try(expr_eval(e, trx(x)));
}

void grf_draw() {
    int x, y, i, lasty;
    int x0 = (int)roundf(itrx(0)), y0 = (int)roundf(itry(0));
    for(i = 0; i < graphscr->size; ++i) {
        graphscr->color[i] = GRAPH_AXES;
    }
    br_clear(graphscr);

    attron(COLOR_PAIR(GRAPH_AXES));

    for(y = 0; y < screen.height; ++y) {
        move(y, 0);
        for(x = 0; x < screen.width; ++x) {
            addch(' ');
        }
    }

    br_colorline(graphscr, 0, y0, br_width(graphscr) - 1, y0, GRAPH_AXES);
    br_colorline(graphscr, x0, 0, x0, br_height(graphscr) - 1, GRAPH_AXES);

    for(y = 0; y < br_height(graphscr); ++y) {
        br_setstate(graphscr, x0, y, 1); 
    }
    
    for(i = 0; i < graphcount; ++i) {
        lasty = (int)roundf(plot(graphs[i].expression, -1));
        for(x = 0; x < br_width(graphscr); ++x) {
            y = (int)roundf(plot(graphs[i].expression, x));
            br_colorline(graphscr, x - 1, lasty, x, y, getcolorcode(graphs[i].color));
            lasty = y;
        }
    }

    br_overlaycurse(graphscr);
    
    refresh();	
}

static void updatescale() {
    sclx = pow(1.1, -sclex); /* Invert sclx so +scale -> zooming in */
    scly = pow(1.1, scley); /* scly needs to be the inverse of sclx */
    
    grf_draw(); /* Refresh graph screen */
}

static graph newgraph(expr *e) {
    if(e == NULL) {
        return err;
    }
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
    if(e == NULL) {
        return err;
    }

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

void grf_center() {
    panx = (screen.width * BR_XFAC) / 2.0f ; /* Make sure to divide by floats */
	pany = (screen.height * BR_YFAC) / 2.0f;
}

void grf_init() {
    graphs = NULL;
    graphcount = 0;
    graphalloc = 0;
    
    cfg cf = cfg_open("colors","r");

    int color_bg = cfg_read(cf, "color_bg", BG_DEFAULT);
    int color_axes = cfg_read(cf, "color_axes", AXES_DEFAULT);
    int color_curve1 = cfg_read(cf, "color_curve1", CURVE1_DEFAULT);
    int color_curve2 = cfg_read(cf, "color_curve2", CURVE2_DEFAULT);
    int color_curve3 = cfg_read(cf, "color_curve3", CURVE3_DEFAULT);

    cfg_free(cf);

    init_pair(GRAPH_AXES, color_axes, color_bg);
    init_pair(GRAPH_RED, color_curve1, color_bg);
    init_pair(GRAPH_GREEN, color_curve2, color_bg);
    init_pair(GRAPH_BLUE, color_curve3, color_bg);

    graphscr = br_scrfromcurse();

    grf_center();
	
	sclex = scley = 24;
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
