#include <curses.h>
#include <math.h>
#include <string.h>

int colors;

float panx, pany;
float sclx, scly;

int sclex, scley;

int scrw, scrh;

int (*mode)(void);

char cmd[256];
int cmdc;

float trx(float x) {
    return sclx * (x - panx);
}

float itrx(float x) {
    return (x / sclx) + panx;
}

float try(float y) {
    return pany - (scly * y);
}

float itry(float y) {
    return (pany - y);
}

float eval(float x) {
    return sin(x);
    //return x * x;
}

float plot(float x) {
    return try(eval(trx(x)));
}

void swap(float *a, float *b) {
    float tmp = *a;
    *a = *b;
    *b = tmp;
}

void draw() {
    int x, y;
    float self, miny, maxy;
    float x0 = itrx(0), y0 = itry(0);
    clear();
    
    for(x = 0; x < scrw; ++x) {
        self = plot(x);
        miny = (plot(x - 1) + self) / 2;
        maxy = (plot(x + 1) + self) / 2;
        if(maxy < miny) {
            swap(&maxy, &miny);
        }
        if(miny > self) { miny = self; }
        if(maxy < self) { maxy = self; }
        for(y = 0; y < scrh; ++y) {
            if((miny <= y && y <= maxy) || (fabs(y - self) <= 0.5)) {
                attron(COLOR_PAIR(2));
                mvaddch(y,x,'*');
            }
            else {
                attron(COLOR_PAIR(1));
                if (fabs(x - x0) <= 0.5) {  
                    mvaddch(y,x,(fabs(y - y0) <= 0.5) ? '+' : '|');
                }
                else if (fabs(y - y0) <= 0.5) {
                    mvaddch(y,x,'-');
                }
                else {
                    attron(COLOR_PAIR(1));
                    mvaddch(y,x,' ');
                }
            }
        }
    }
    
    
    refresh();	
}

void scale() {
    sclx = pow(1.1, -sclex); /* Invert sclx so +scale -> zooming in */
    scly = pow(1.1, scley); /* scly needs to be the inverse of sclx */
    
    draw(); /* Refresh graph screen */
}

void getsize() {
    getmaxyx(stdscr,scrh,scrw);
    //if(scrw > 3 * scrh) {
    //    scrw = 3 * scrh;
    //}
}

void drawterm() {
    int x;
    draw();
    attron(COLOR_PAIR(3));
    mvaddch(scrh - 1, 0, '>');
    addch(' ');
    for(x = 0; x < scrw - 2; ++x) {
        addch(cmd[x]);
    }
    refresh();
}

int mode_term(void), mode_graph(void);

int isvalid(int ch) { return ch >= 32 && ch <= 126; }

int mode_term() {
    int ch = getch();
    if(ch == 10) {
        cmd[cmdc] = '\0';
        if(!strcmp(cmd, "quit")) { return 0; }
        if(!strcmp(cmd, "center")) { panx = scrw / 2; pany = scrh / 2;}
        mode = &mode_graph;
        draw();
        return 1;
    }
    else if(ch == '\033') {
        mode = &mode_graph;
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
}

int mode_graph() {
    int ch = getch();
    switch(ch) {
        case 'e':
            ++sclex;
            ++scley;
            scale();
            break;
        case 'r':
            --sclex;
            --scley;
            scale();
            break;
        case 'w':
            ++pany;
            draw();
            break;
        case 's':
            --pany;
            draw();
            break;
        case 'a':
            ++panx;
            draw();
            break;
        case 'd':
            --panx;
            draw();
            break;
        case '.':
        case '>':
            mode = &mode_term;
            drawterm();
            cmdc = 0;

    }
    return ch != 'q';
}

int main()
{	
    int ch;
    
	initscr();
	
	//keypad(stdscr, true);
	
	noecho();
	cbreak();
    curs_set(0);
	
	if(has_colors()) {
	    start_color();
	    
	    colors = 1;
	    init_pair(1, COLOR_BLACK, COLOR_WHITE);
        init_pair(2, COLOR_RED, COLOR_WHITE);
        init_pair(3, COLOR_WHITE, COLOR_BLACK);
	    attron(COLOR_PAIR(1));
	    //printw("Has colors!");
	}
	
	getsize();
	panx = scrw / 2.0f; /* Make sure to divide by floats */
	pany = scrh / 2.0f;
	
	sclex = scley = 20;
	scale();

    int i;
    for(i = 0; i < 256; ++i) { cmd[i] = ' '; }
	
	mode = &mode_graph;
    while(mode());
	
	endwin();

	return 0;
}
