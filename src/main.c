#include <ncurses.h>
#include <stdlib.h>
#include <locale.h>
#include <signal.h>

#include "expression.h"
#include "graph.h"
#include "terminal.h"

#include "colormenu.h"
#include "setup.h"

#include "base.h"

static void (*callback)(void);

void on_sigsegv(int signo) {
    quit(-1);
}

int main(int argc, char **argv)
{	
    setlocale(LC_ALL, ""); /* Required for unicode characters */

    sigset(SIGSEGV, on_sigsegv);

    if(argc > 1) { //TODO: Implement better command-line options
        if(!strcmp(argv[1], "--setup")) {
            setup_run();
            return 0;
        }
        else if(!strcmp(argv[1], "--help")) {
            puts("FADS - Graphing calculator for the terminal\n");
            puts("Command line options:");
            
            puts("--color-config: run the FADS color configuration utility");
            puts("--help: show this menu");
            puts("--setup: overwrite all config files with default settings");
            return 0;
        }
        else if(strcmp(argv[1], "--color-config")) {
            printf("Unknown option \"%s\". Use --help for help.\n", argv[1]);
            return 0;
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

    if(argc > 1) {
        if(!strcmp(argv[1], "--color-config")) {
            ccfg_run();
            return 0;
        }
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

    if(status == -1) {
        puts("Quitting: Segmentation fault.");
    }

    exit(status);
}
