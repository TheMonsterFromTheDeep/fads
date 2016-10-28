#include <curses.h>
#include "base.h"
#include "graph.h"
#include "expression.h"
#include "equation.h"

#include "terminal.h"

#define CMD_SIZE 256 /* TODO: Replace with infinite string thing */
static char cmd[CMD_SIZE];
static int cmdc; /* size of cmd */

static int isvalid(int ch) { return ch >= 32 && ch <= 126; }

void term_init(void) {
    init_pair(TERM_TEXT, COLOR_WHITE, COLOR_BLACK);
}

void term_activate(void) {
    int i;
    for(i = 0; i < CMD_SIZE; ++i) {
        cmd[i] = 0;
    }
    cmdc = 0;

    term_draw();
}

void term_draw(void) {
    int x;
    
    grf_draw(); /* Draw graph */
    
    attron(COLOR_PAIR(TERM_TEXT));
   
    mvaddch(screen.height - 1, 0, '>'); /* Draw input caret */

    addch(' '); /* Add a space before command */
    
    for(x = 0; x < screen.width - 3; ++x) {
        addch(x < cmdc ? cmd[x] : ' '); /* Draw command */
    }

    refresh();
}

void term_loop(void) {
    int ch = getch();
    if(ch == 10) {
        cmd[cmdc] = '\0';
        //if(!strcmp(cmd, "quit")) { /* TODO: Command parsing */
        //    quit(0);
        //}
        //if(!strcmp(cmd, "center")) { grf_center(); }
        //if(!strcmp(cmd, "testadd")) { grf_addgraph(expr_new_const(3)); }
       // if(!strcmp(cmd, "testrm")) { grf_removegraph(1); }
        grf_addgraph(eq_parse(cmd));
        mode_set(GRAPH);
        return;
    }
    else if(ch == '\033') {
        mode_set(GRAPH);
        return;
    }
    else if(ch == 127) {
        if(cmdc > 0) {
            cmd[--cmdc] = ' ';
        }
    }
    else if(isvalid(ch) && cmdc < CMD_SIZE) {
        cmd[cmdc++] = ch;
    }
    term_draw();
}

#undef CMD_SIZE
