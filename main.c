#include <curses.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#include "expression.h"
#include "graph.h"

int colors; /* TODO: Move this stuff to a common base file.. */

int (*mode)(void);

char cmd[256];
int cmdc;

/*void drawterm() {
    int x;
    draw();
    attron(COLOR_PAIR(3));
    mvaddch(scrh - 1, 0, '>');
    addch(' ');
    for(x = 0; x < scrw - 2; ++x) {
        addch(cmd[x]);
    }
    refresh();
}*/

int mode_term(void);

int isvalid(int ch) { return ch >= 32 && ch <= 126; }

/*int mode_term() {
    int ch = getch();
    if(ch == 10) {
        cmd[cmdc] = '\0';
        if(!strcmp(cmd, "quit")) { return 0; }
        if(!strcmp(cmd, "center")) { panx = scrw / 2; pany = scrh / 2;}
        mode = &grf_loop; /* TODO: Replace with setmode() function
        draw();
        return 1;
    }
    else if(ch == '\033') {
        //mode = &mode_graph;
        draw();
        return 1;
    }
    else if(ch == 127) {
        if(cmdc > 0) {
            cmd[--cmdc] = ' ';
        }
    }
    else if(isvalid(ch)) {
        cmd[cmdc++] = ch;
    }
    drawterm();
    return 1;
} */ /* TODO: Make actual terminal thingy */

int main()
{	
	initscr();
	
	noecho();
	cbreak();
    curs_set(0);

	if(has_colors()) {
	    start_color();
        colors = 1;
	}

    grf_init();
	
	mode = &grf_loop;
    while(mode());
	
	endwin();

    grf_end();

	return 0;
}
