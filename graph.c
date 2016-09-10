#include <stdlib.h>
#include <curses.h>
#include <math.h>

#include "base.h"
#include "graph.h"
#include "expression.h"

static graph *graphs;
static int graphcount;

static int currentcolor;

static float panx, pany;
static float sclx, scly;

static int sclex, scley;

static int scrw, scrh;

void getsize() {
    getmaxyx(stdscr,scrh,scrw);
}

int nextcolor() {
    if(currentcolor < COLOR_GRAPH_COUNT) { ++currentcolor; }
    else { currentcolor = 0; }
    return currentcolor;
}

int getcolorcode(int color) {
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

float trx(float x) {
    return sclx * (x - panx);
}

float itrx(float x) {
    return (x / sclx) + panx;
}

float try(float y) {
    return pany - (scly * y);
}

float itry(float y) {
    return (pany - y);
}

void swap(float *a, float *b) {
    float tmp = *a;
    *a = *b;
    *b = tmp;
}

float plot(expr *e, float x) {
    return try(expr_eval(e, trx(x)));
}

void grf_draw() {
    int x, y, i;
    float self, miny, maxy;
    float x0 = itrx(0), y0 = itry(0);
    float *yvals = malloc(sizeof(float) * graphcount), *mins = malloc(sizeof(float) * graphcount), *maxes = malloc(sizeof(float) * graphcount); /* This will probably be moved out of draw eventually */
    clear();
    
    for(x = 0; x < scrw; ++x) {
        for(i = 0; i < graphcount; ++i) {
            self = plot(graphs[i].expression, x);
            miny = (plot(graphs[i].expression, x - 1) + self) / 2;
            maxy = (plot(graphs[i].expression, x + 1) + self) / 2;
            if(maxy < miny) {
                swap(&maxy, &miny);
            }
            if(miny > self) { miny = self; }
            if(maxy < self) { maxy = self; }
            yvals[i] = self;
            mins[i] = miny;
            maxes[i] = maxy;
        }
        for(y = 0; y < scrh; ++y) {
            for(i = 0; i < graphcount; ++i) {
                if((mins[i] <= y && y <= maxes[i]) || (fabs(y - yvals[i]) <= 0.5)) {
                    attron(COLOR_PAIR(getcolorcode(graphs[i].color)));
                    mvaddch(y,x,'*');
                }
                else {
                    attron(COLOR_PAIR(1));
                    if (fabs(x - x0) <= 0.5) {  
                        mvaddch(y,x,(fabs(y - y0) <= 0.5) ? '+' : '|');
                    }
                    else if (fabs(y - y0) <= 0.5) {
                        mvaddch(y,x,'-');
                    }
                    else {
                        attron(COLOR_PAIR(GRAPH_AXES));
                        mvaddch(y,x,' ');
                    }
                }
            }
        }
    }
    
    
    refresh();	
}

static void updatescale() {
    sclx = pow(1.1, -sclex); /* Invert sclx so +scale -> zooming in */
    scly = pow(1.1, scley); /* scly needs to be the inverse of sclx */
    
    grf_draw(); /* Refresh graph screen */
}

graph newgraph(expr *e) {
    graph g = {
        e, 
        nextcolor()
    };
    return g;
}

void grf_init() {
    graphs = malloc(sizeof(graph));
    graphcount = 1;
    graphs[0] = newgraph(expr_new_add(expr_new_x(), expr_new_const(3))); /* DEBUG (could add default graph later) */
    getsize();

    init_pair(GRAPH_AXES, COLOR_BLACK, COLOR_WHITE);
    init_pair(GRAPH_RED, COLOR_RED, COLOR_WHITE);
    init_pair(GRAPH_GREEN, COLOR_GREEN, COLOR_WHITE);
    init_pair(GRAPH_BLUE, COLOR_BLUE, COLOR_WHITE);

    panx = scrw / 2.0f; /* Make sure to divide by floats */
	pany = scrh / 2.0f;
	
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

void grf_pan(float x, float y) {
    panx = x;
    pany = y;
    grf_draw();
}

void grf_panby(float dx, float dy) {
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
        //case '.': /* This will be reimplemented soon... */
        //case '>':
        //    mode = &mode_term;
        //    drawterm();
        //    cmdc = 0;

    }
}

void grf_end() {
    int i;
    for(i = 0; i < graphcount; ++i) {
        expr_free(graphs[i].expression);
    }
    free(graphs);
}
