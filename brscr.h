#ifndef BRSCR_H
#define BRSCR_H

#define BR_XFAC 2
#define BR_YFAC 4

typedef struct brscr {
    int *data;
    int *color;
    int width;
    int height;
    int size;
} brscr;

brscr *br_scrfromcurse(void);
void br_free(brscr*);
void br_clear(brscr*);

void br_setstate(brscr*,int,int,int);
void br_colorline(brscr*,int,int,int,int,int);

int br_width(brscr*);
int br_height(brscr*);
int br_xyoffset(brscr*,int,int);

void br_drawtocurse(brscr*);
void br_overlaycurse(brscr*);

#endif
