#include <curses.h>
#include <math.h>

int colors;

float panx, pany;
float sclx, scly;

int sclex, scley;

int scrw, scrh;

float eval(float x) {
    return sin(x);
}

float plot(float x) {
    return pany - scly * eval(sclx * (x - panx));
}

void swap(float *a, float *b) {
    float tmp = *a;
    *a = *b;
    *b = tmp;
}

void draw() {
    clear();
    
    
    for(int x = 0; x < scrw; ++x) {
        float self = plot(x);
        float miny = (plot(x - 1) + self) / 2;
        float maxy = (plot(x + 1) + self) / 2;
        if(maxy < miny) {
            swap(&maxy, &miny);
        }
        if(miny > self) { miny = self; }
        if(maxy < self) { maxy = self; }
        for(int y = 0; y < scrh; ++y) {
            mvaddch(y,x,(miny <= y && y <= maxy) || (fabs(y - self) <= 0.5) ? 'O' : ' ');
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
    if(scrw > 3 * scrh) {
        scrw = 3 * scrh;
    }
}

int main()
{	
    int ch;
    
	initscr();
	
	//keypad(stdscr, true);
	
	
	
	noecho();
	raw();
	
	if(has_colors()) {
	    start_color();
	    
	    colors = 1;
	    init_pair(1, COLOR_RED, COLOR_BLACK);
	    attron(COLOR_PAIR(1));
	    //printw("Has colors!");
	}
	
	getsize();
	panx = scrw / 2.0f; /* Make sure to divide by floats */
	pany = scrh / 2.0f;
	
	sclex = scley = 20;
	scale();
	
	
	
    while((ch = getch()) != 'q') {
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
        }
    }
	
	endwin();

	return 0;
}
