#ifndef GRAPHER_H
#define GRAPHER_H

#include "base.h"
#include "expression.h"

void grf_init(void);

void grf_draw(void);
void grf_loop(void);

void grf_scale(int, int);
void grf_scaleby(int, int);
void grf_pan(num, num);
void grf_panby(num, num);
void grf_center(void);

void grf_end(void);

#endif
