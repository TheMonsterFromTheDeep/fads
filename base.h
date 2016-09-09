#ifndef BASE_H
#define BASE_H

#define COLOR_GRAPH_AXES 1 /* TODO: Look into enums for this */
#define COLOR_GRAPH_RED 2
#define COLOR_GRAPH_GREEN 3
#define COLOR_GRAPH_BLUE 4

#define COLOR_GRAPH_COUNT 3

typedef enum mode {
    GRAPH,
    TERMINAL
} mode;

void mode_set(mode);

void quit(int);

#endif
