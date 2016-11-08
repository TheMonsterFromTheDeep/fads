#ifndef BASE_H
#define BASE_H

#define COLOR_GRAPH_COUNT 3

typedef enum mode {
    GRAPH,
    EDITOR,
    TERMINAL
} mode;

typedef enum colors {
    GRAPH_AXES = 1,
    GRAPH_RED,
    GRAPH_GREEN,
    GRAPH_BLUE,
    TERM_TEXT
} colors;

void init(void);

void mode_set(mode);
void mode_return(void);

void quit(int);

struct {
    int width;
    int height;
} screen;

typedef float num;

#define isvalid(ch) (ch >= 32 && ch <= 126)

#endif
