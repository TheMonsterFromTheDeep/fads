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

int br_xyoffset(brscr *scr, int x, int y) {
    return x + y * scr->width;
}

void br_setstate(brscr *scr, int x, int y, int state) {
    if(x < 0 || x >= br_width(scr)) { return; }
    if(y < 0 || y >= br_height(scr)) { return; }
    int *mod = scr->data + br_xyoffset(scr, x / BR_XFAC, y / BR_YFAC);
    x = x % BR_XFAC;
    y = y % BR_YFAC;
    if(state) {
        *mod = ((*mod - 0x2800) | xymask(x, y)) + 0x2800;
    }
    else {
        *mod = ((*mod - 0x2800) & ~xymask(x, y)) + 0x2800;
    }
}

void br_setcolor(brscr *scr, int x, int y, int color) {
    if(x < 0 || x >= br_width(scr)) { return; }
    if(y < 0 || y >= br_height(scr)) { return; }
    scr->color[br_xyoffset(scr, x / BR_XFAC, y / BR_YFAC)] = color;
}

void br_setstco(brscr *scr, int x, int y, int state, int color) {
    if(x < 0 || x >= br_width(scr)) { return; }
    if(y < 0 || y >= br_height(scr)) { return; }
    scr->color[br_xyoffset(scr, x / BR_XFAC, y / BR_YFAC)] = color;
    int *mod = scr->data + br_xyoffset(scr, x / BR_XFAC, y / BR_YFAC);
    x = x % BR_XFAC;
    y = y % BR_YFAC;
    if(state) {
        *mod = ((*mod - 0x2800) | xymask(x, y)) + 0x2800;
    }
    else {
        *mod = ((*mod - 0x2800) & ~xymask(x, y)) + 0x2800;
    }
}

static void plotoctant(brscr *scr, int x, int y, int octant, int color) {
    int outx, outy;
    switch(octant) {
        case 0:
            outx = x;
            outy = y;
            break;
        case 1:
            outx = y;
            outy = x;
            break;
        case 2:
            outx = -y;
            outy = x;
            break;
        case 3:
            outx = -x;
            outy = y;
            break;
        case 4:
            outx = -x;
            outy = -y;
            break;
        case 5:
            outx = -y;
            outy = -x;
            break;
        case 6:
            outx = y;
            outy = -x;
            break;
        case 7:
            outx = x;
            outy = -y;
    }
    br_setstco(scr, outx, outy, 1, color);
}

static void brensenham(brscr *scr, int x0, int y0, int x1, int y1, int octant, int color) {
    int dx = x1 - x0;
    int dy = y1 - y0;
    int D = 0;
    int y = y0;
    int x;

    for (x = x0; x <= x1; ++x) {
        plotoctant(scr, x, y, octant, color);
        //br_setstco(scr, x, y, 1, color);
        D += dy;
        //
        if ((D << 1) >= dx) {
            ++y;
            D -= dx;
        }
        
    }
}

static int zerox(int x, int y, int octant) {
    switch(octant) {
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return y;
        case 3:
            return -x;
        case 4:
            return -x;
        case 5:
            return -y;
        case 6:
            return -y;
        case 7:
            return x;
    }
}

static int zeroy(int x, int y, int octant) {
    switch(octant) {
        case 0:
            return y;
        case 1:
            return x;
        case 2:
            return -x;
        case 3:
            return y;
        case 4:
            return -y;
        case 5:
            return -x;
        case 6:
            return x;
        case 7:
            return -y;
    }
}

void br_colorline(brscr *scr, int x0, int y0, int x1, int y1, int color) {
    int octant;

    if( (x0 < 0 && x1 < 0) ||
        (y0 < 0 && y1 < 0) ||
        (x0 > br_width(scr) && x1 > br_width(scr)) ||
        (y0 > br_height(scr) && y1 > br_height(scr)) ) { return; }

    float m;

    if(x1 - x0 != 0) {
        m = ((float)(y1 - y0)) / (x1 - x0);
        if(x0 < 0) {
            y0 -= (int)(m * -x0);
            x0 = 0;
        }
        else if(x0 > br_width(scr)) {
            y0 -= (int)(m * (x0 - br_width(scr)));
            x0 = br_width(scr);
        }
        if(x1 < 0) {
            y1 -= (int)(m * -x1);
            x1 = 0;
        }
        else if(x1 > br_width(scr)) {
            y1 -= (int)(m * (x1 - br_width(scr)));
            x1 = br_width(scr);
        }
    }

    if(y1 - y0 != 0) {
        m = ((float)(x1 - x0)) / (y1 - y0);
        if(y0 < 0) {
            x0 -= (int)(m * -y0);
            y0 = 0;
        }
        else if(y0 > br_height(scr)) {
            x0 -= (int)(m * (y0 - br_height(scr)));
            y0 = br_height(scr);
        }
        if(y1 < 0) {
            x1 -= (int)(m * -y1);
            y1 = 0;
        }
        else if(y1 > br_height(scr)) {
            x1 -= (int)(m * (y0 - br_height(scr)));
            y1 = br_height(scr);
        }
    }

    int dx = x1 - x0;
    int dy = y1 - y0;

    

    if(dx >= 0) {
        octant = dy > 0 ? dy > dx :
                 dy < 0 ? 6 + (-dy < dx) :
                 0;
    }
    else if(dx < 0) {
        octant = dy > 0 ? 2 + (dy < -dx) :
                 dy < 0 ? 4 + (-dy > -dx) :
                 3;
    }
    int drx0 = zerox(x0, y0, octant);
    int dry0 = zeroy(x0, y0, octant);
    int drx1 = zerox(x1, y1, octant);
    int dry1 = zeroy(x1, y1, octant);
    brensenham(scr, drx0, dry0, drx1, dry1, octant, color);
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
    nscr->color = malloc(sizeof(int) * nscr->size);
    br_clear(nscr);
    return nscr;
}

void br_free(brscr *scr) {
    free(scr->data);
    free(scr->color);
    free(scr);
}

void br_overlaycurse(brscr *scr) {
    int x, y;
    int inc = 0;
    for(y = 0; y < scr->height; ++y) {
        for(x = 0; x < scr->width; ++x) {
            if(scr->data[br_xyoffset(scr, x, y)] != 0x2800) {
                move(y, x);
                attron(COLOR_PAIR(scr->color[br_xyoffset(scr, x, y)]));
                printw("%lc",scr->data[br_xyoffset(scr, x, y)]);
            }
        }
    }
}

void br_drawtocurse(brscr *scr) {
    int x, y;
    int inc = 0;
    for(y = 0; y < scr->height; ++y) {
        move(y, 0);
        for(x = 0; x < scr->width; ++x) {
            attron(COLOR_PAIR(scr->color[br_xyoffset(scr, x, y)]));
            printw("%lc",scr->data[br_xyoffset(scr, x, y)]);
        }
    }
}
