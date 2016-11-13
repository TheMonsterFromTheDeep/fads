#include <ncurses.h>
#include <stdlib.h>
#include <locale.h>
#include <signal.h>
#include <zlib/zentry.h>

#include "expression.h"
#include "grapher.h"
#include "terminal.h"
#include "editor.h"
#include "graph.h"

#include "colormenu.h"
#include "setup.h"

#include "base.h"

static void (*callback)(void);

static mode currentMode;
static mode lastMode;

void entry(zargs args) {
    set_exit_handler(&quit); /* Set exit handler */

    setlocale(LC_ALL, ""); /* Required for unicode characters */

    if(args.count > 1) { //TODO: Implement better command-line options
        if(!strcmp(args.get[1], "--setup")) {
            setup_run();
            return;
        }
        else if(!strcmp(args.get[1], "--help")) {
            puts("FADS - Graphing calculator for the terminal\n");
            puts("Command line options:");
            
            puts("--color-config: run the FADS color configuration utility");
            puts("--help: show this menu");
            puts("--setup: overwrite all config files with default settings");
            return;
        }
        else if(strcmp(args.get[1], "--color-config")) {
            printf("Unknown option \"%s\". Use --help for help.\n", args.get[1]);
            return;
        }
    }

	initscr(); /* Curses init statement */
	
	noecho(); /* Set up curses mode */
	cbreak();
    curs_set(0);

    keypad(stdscr, TRUE);

	if(has_colors()) {
	    start_color(); /* Start curses color if possible */
	}

    if(args.count > 1) {
        if(!strcmp(args.get[1], "--color-config")) {
            ccfg_run();
            return;
        }
    }

    graph_setup(); /* Setup graph data */

    init(); /* Init main program */
    grf_init(); /* Init graph module */
    term_init(); /* Init term module */
    ed_init(); /* Init editor module */
	
	mode_set(GRAPH); /* Set initial mode to graph */

    lastMode = EDITOR; /* Initial mode to switch to on TAB */

    for(;;) { /* Main loop */
        callback();
    }
}

void init(void) {
    getmaxyx(stdscr,screen.height,screen.width);
}

void mode_set(mode m) {
    lastMode = currentMode;
    currentMode = m;
    switch(m) {
        case GRAPH:
            callback = &grf_loop;
            grf_draw();
            break;
        case TERMINAL:
            callback = &term_loop;
            term_activate(); /* TODO: Implement structured system for this */
            break;
        case EDITOR:
            callback = &ed_loop;
            ed_activate();
            break;
    }
}

void mode_return() {
    mode_set(lastMode);
}

int quit() {
    endwin(); /* Exit curses mode */

    grf_end(); /* Tell graphing module to exit */

    graph_freeall(); /* Free graph data */

    exit(0);
}
