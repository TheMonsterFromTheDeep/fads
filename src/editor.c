#include <ncurses.h>
#include <stdlib.h>

#include "base.h"
#include "expression.h"
#include "grapher.h"
#include "editor.h"
#include "graph.h"

#include "equation.h"

static size_t current;

typedef struct equation { 
    char data[256];
    size_t length;
} equation;
static equation *equations;
static size_t eq_count;

void ed_init() {
    size_t i;

    equations = malloc(sizeof(equation));
    eq_count = 1;

    for(i = 0; i < 256; ++i) {
        equations[0].data[i] = '\0';
    }
    equations[0].length = 0;

    current = 0;

    zlist_add(graphs, graph_create(NULL));
}

void ed_draw() {
    int x, y;
    size_t eq, eqc;

    grf_draw();

    attron(COLOR_PAIR(GRAPH_AXES));

    eqc = 0;

    for(y = 0; y < screen.height; ++y) {
        move(y, 0);
        if((y - 1) / 2 < eq_count) {
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
                    if(equations[eqc].data[eq] != '\0') {
                        addch(equations[eqc].data[eq++]);
                    }
                    else { addch(' '); }
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
    int ch = getch();
    if(ch == '\t') { /* Tab */
        grf_panby(screen.width / -2, 0);
        mode_set(GRAPH);
        return;
    }
    else if(ch == 127 || ch == KEY_BACKSPACE) {
        if(equations[current].length > 0) {
            equations[current].data[--equations[current].length]= '\0';
            graphs->expression = eq_parse(equations[current].data);
        }
    }
    else if(isvalid(ch)) { /* TODO: Create a generalized typing module for this & terminal */
        if(equations[current].length < 255) {
            equations[current].data[equations[current].length++] = (char)ch;
            expr *new = eq_parse(equations[current].data);
            graphs->expression = new;
        }
    }
    ed_draw();
}
