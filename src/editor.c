#include <ncurses.h>
#include <stdlib.h>

#include "base.h"
#include "expression.h"
#include "grapher.h"
#include "editor.h"
#include "graph.h"

#include "equation.h"

static size_t current;
static size_t cursor;

void ed_init() {
    current = 0;

    zlist_add(graphs, graph_create());
}

void ed_draw() {
    int x, y;
    size_t eq, eqc;

    grf_draw();

    attron(COLOR_PAIR(GRAPH_AXES));

    eqc = 0;

    for(y = 0; y < screen.height; ++y) {
        move(y, 0);
        if((y - 1) / 2 < zlist_size(graphs)) {
            if (y % 2 == 0) {
                for(x = 0; x < screen.width / 2; ++x) {
                    addch('-');
                }
            }
            else {
                graph g = zlist_get(graphs, eqc);
                if(graph_valid(g)) { 
                    attron(COLOR_PAIR(g.color));
                    addstr("[@]");
                    attron(COLOR_PAIR(GRAPH_AXES));
                }
                else {
                    addstr("[!]");
                }
                
                addch(' ');
                eq = 0;
                for(x = 4; x < screen.width / 2; ++x) {
                    if(eq == g.equation->size && eqc == current) {
                        attron(A_REVERSE);
                    }
                    if(eq < g.equation->size) {
                        addch(zstr_at(g.equation, eq));
                    }
                    else { addch(' '); }
                    attroff(A_REVERSE);
                    ++eq;
                }
                ++eqc;
            }
        }
        else {
            for(x = 0; x < screen.width / 2; ++x) {
                addch(' ');
            }
        }
        addch('|');
    }

    refresh();
}

void ed_activate() {
    grf_panby(screen.width / 2, 0);
    ed_draw();
}

void ed_loop() {
    graph *cg = zlist_get_ptr(graphs, current);

    int ch = getch();
    if(ch == '\t') { /* Tab */
        grf_panby(screen.width / -2, 0);
        mode_set(GRAPH);
        return;
    }
    else if(ch == 127 || ch == KEY_BACKSPACE) {
        zstr_backspace(cg->equation);
        graph_update(current);
    }
    else if(isvalid(ch)) { /* TODO: Create a generalized typing module for this & terminal */
        zstr_catb(cg->equation,(char)ch);
        graph_update(current);
    }
    else if(ch == KEY_DOWN || ch == '\n') {
        current++;
        if(current >= zlist_size(graphs)) {
            zlist_add(graphs, graph_create());
        }
    }
    else if(ch == KEY_UP) {
        if(current > 0) { --current; }
    }
    ed_draw();
}
