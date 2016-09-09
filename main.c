#include <curses.h>
#include <stdlib.h>

#include "expression.h"
#include "graph.h"

#include "base.h"

static void (*callback)(void);

int main()
{	
	initscr(); /* Curses init statement */
	
	noecho(); /* Set up curses mode */
	cbreak();
    curs_set(0);

	if(has_colors()) {
	    start_color(); /* Start curses color if possible */
	}

    grf_init(); /* Init graph module */
	
	mode_set(GRAPH); /* Set initial mode to graph */

    for(;;) { /* Main loop */
        callback();
    }
}

void mode_set(mode m) {
    switch(m) {
        case GRAPH:
            callback = &grf_loop;
            break;
        case TERMINAL:
            break;
    }
}

void quit(int status) {
    endwin(); /* Exit curses mode */

    grf_end(); /* Tell graphing module to exit */

    exit(status);
}
