#include <ncurses.h>
#include <stdlib.h>
#include "brscr.h"

/*
Character map:
0x2800
+0 +3
+1 +4
+2 +5
+6 +7
*/

int br_width(brscr *scr) {
    return scr->width * BR_XFAC;
}

int br_height(brscr *scr) {
    return scr->height * BR_YFAC;
}

static int xymask(int x, int y) {
    if (y > 2) {
        return 1 << 6 + x;
    }
    return 1 << (x * 3) + y;
}

static int xyoffset(brscr *scr, int x, int y) {
    return x + y * scr->width;
}

void br_setstate(brscr *scr, int x, int y, int state) {
    if(x < 0 || x >= br_width(scr)) { return; }
    if(y < 0 || y >= br_height(scr)) { return; }
    int *mod = scr->data + xyoffset(scr, x / BR_XFAC, y / BR_YFAC);
    x = x % BR_XFAC;
    y = y % BR_YFAC;
    if(state) {
        *mod = ((*mod - 0x2800) | xymask(x, y)) + 0x2800;
    }
    else {
        *mod = ((*mod - 0x2800) & ~xymask(x, y)) + 0x2800;
    }
}

void br_clear(brscr *scr) {
    int i;
    for(i = 0; i < scr->size; ++i) { scr->data[i] = 0x2800; }
}

brscr *br_scrfromcurse() {
    brscr *nscr = malloc(sizeof(brscr));
    getmaxyx(stdscr,nscr->height,nscr->width);
    nscr->size = nscr->width * nscr->height;
    nscr->data = malloc(sizeof(int) * nscr->size);
    br_clear(nscr);
    return nscr;
}

void br_free(brscr *scr) {
    if(scr->size > 0) {
        free(scr->data);
    }
    free(scr);
}

void br_drawtocurse(brscr *scr) {
    int x, y;
    int inc = 0;
    for(y = 0; y < scr->height; ++y) {
        move(y, 0);
        for(x = 0; x < scr->width; ++x) {
            printw("%lc",scr->data[xyoffset(scr, x, y)]);
        }
    }
}
