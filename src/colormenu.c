#include <ncurses.h>
#include <string.h>
#include <stdlib.h>

#include "colormenu.h"
#include "config.h"
#include "colordefaults.h"

static int position = 0;

static int *curval;
static int curmod;

void addopt(const char *str, int pos, int *val, int mod) {
    addch(pos == position ? '>' : ' ');
    addstr(str);
    addch(pos == position ? '<' : ' ');
    if(pos == position) {
        curval = val;
        curmod = mod;
    }
}

void addcolor(const char *str, int pos, int *val) {
    int i;
    addopt(str, pos, val, COLORS + 1);
    for(i = 0; i < 15 - strlen(str); ++i) { addch(' '); }
    attron(COLOR_PAIR(*val));
    addstr(" \n");
    attroff(COLOR_PAIR(*val));
}

void exportsettings(int colors, int bg, int axes, int c1, int c2, int c3) {
    cfg cf = cfg_open("colors","w");
    cfg_clear(cf);
    cfg_writeprop(cf, "colors_enabled", colors);
    cfg_writeprop(cf, "color_bg", bg);
    cfg_writeprop(cf, "color_axes", axes);
    cfg_writeprop(cf, "color_curve1", c1);
    cfg_writeprop(cf, "color_curve2", c2);
    cfg_writeprop(cf, "color_curve3", c3);
    cfg_free(cf);
}

void ccfg_run() {
    cfg cf = cfg_open("colors", "r");
    
    int 
      colorstate = cfg_read(cf, "colors_enabled", COLORSTATE_DEFAULT),
      graphbg = cfg_read(cf, "color_bg", BG_DEFAULT),
      graphaxes = cfg_read(cf, "color_axes", AXES_DEFAULT),
      graphcolor1 = cfg_read(cf, "color_curve1", CURVE1_DEFAULT),
      graphcolor2 = cfg_read(cf, "color_curve2", CURVE2_DEFAULT),
      graphcolor3 = cfg_read(cf, "color_curve3", CURVE3_DEFAULT);

    cfg_free(cf);

    int i;

    int move;

    for(i = 1; i < COLORS; ++i) {
        init_pair(i, i, i);
    }

    for(;;) {
        move(0, 0);
        addstr("FADS Color Configuration Utility\n\n");

        addopt("Color:", 0, &colorstate, 2);
        addstr("         ");
        addstr(colorstate ? "ON" : "OFF");
        addch('\n');

        addcolor("Graph BG:", 1, &graphbg);
        addcolor("Axes Color:", 2, &graphaxes);
        addcolor("Curve Color 1:", 3, &graphcolor1);
        addcolor("Curve Color 2:", 4, &graphcolor2);
        addcolor("Curve Color 3:", 5, &graphcolor3);

        addch('\n');

        addopt("Confirm", 6, NULL, 0);
        addch('\n');
        addopt("Cancel", 7, NULL, 0);

        refresh();

        move = getch();
        switch(move) {
            case KEY_DOWN:
                if(position < 7) {
                    ++position;
                }
                break;
            case KEY_UP:
                if(position > 0) {
                    --position;
                }
                break;
            case KEY_LEFT:
                if(curval != NULL) {
                    *curval = (*curval - 1) % curmod;
                }
                break;
            case KEY_RIGHT:
                if(curval != NULL) {
                    *curval = (*curval + 1) % curmod;
                }
                break;
            case KEY_ENTER:
            case 10:
                if(position == 6) {
                    exportsettings(colorstate, graphbg, graphaxes, graphcolor1, graphcolor2, graphcolor3);
                }
                if(position > 5) {
                    endwin();
                    exit(0);
                }
                break;
        }
    }
}
