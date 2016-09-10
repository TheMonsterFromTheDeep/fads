#include <curses.h>
#include <stdlib.h>

#include "expression.h"
#include "graph.h"
#include "terminal.h"

#include "base.h"

static void (*callback)(void);

int main()
{	
    printf("\e]0;%s\007", "TermCalc");
	initscr(); /* Curses init statement */
	
	noecho(); /* Set up curses mode */
	cbreak();
    curs_set(0);

	if(has_colors()) {
	    start_color(); /* Start curses color if possible */
	}

    init(); /* Init main program */
    grf_init(); /* Init graph module */
    term_init(); /* Init term module */
	
	mode_set(GRAPH); /* Set initial mode to graph */

    for(;;) { /* Main loop */
        callback();
    }
}

void init(void) {
    getmaxyx(stdscr,screen.height,screen.width);
}

void mode_set(mode m) {
    switch(m) {
        case GRAPH:
            callback = &grf_loop;
            grf_draw();
            break;
        case TERMINAL:
            callback = &term_loop;
            term_activate(); /* TODO: Implement structured system for this */
            break;
    }
}

void quit(int status) {
    endwin(); /* Exit curses mode */

    grf_end(); /* Tell graphing module to exit */

    exit(status);
}
