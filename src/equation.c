#include <string.h>
#include <stdlib.h>
#include "expression.h"

#include "equation.h"

static expr **outstack;
static int outtop;
static int *opstack;
static int optop;

static int is_num(int c) {
    return c >= '0' && c <= '9';
}

static float read_part(const char *str, int *i, int part) {
    float f = 0;
    int l = 10;
    while(is_num(str[*i]) && str[*i] != '\0') {
        if(part == 'i') {
            f *= 10;
            f += str[*i] - '0';
            
        }
        if(part == 'f') {
            f += (float)(str[*i] - '0') / l;
            l *= 10;
        }
        ++*i;
    }
    
    return f;
}

static int precedence(int op) {
    switch(op) {
        case '*':
        case '/':
        return 1;

        case '^':
        return 0;

        case '+':
        case '-':
        default:
        return 2;
    }
}

static int is_right(int op) {
    switch(op) {
        case '^':
        return 1;

        case '+':
        case '-':
        case '*':
        case '/':
        default:
        return 0;
    }
}

static int should_pop(int op1, int op2) {
    if(op2 == '(') { return 0; }
    if(is_right(op2)) {
        return precedence(op2) < precedence(op1);
    }
    return precedence(op2) <= precedence(op1);
}

static expr *get_op(int op) {
    switch(op) {
        case '+':
            return expr_new_add(outstack[--outtop], outstack[--outtop]);
        case '-':
            return expr_new_sub(outstack[--outtop], outstack[--outtop]);
        case '*':
            return expr_new_mul(outstack[--outtop], outstack[--outtop]);
        case '/':
            return expr_new_div(outstack[--outtop], outstack[--outtop]);
        case '^':
            return expr_new_pow(outstack[--outtop], outstack[--outtop]);
    }
    return NULL;
}

static void pop_ops(int nop) {
    int op;

    while(optop > 0) {
        if(!should_pop(nop, opstack[optop - 1])) { break; }
        op = opstack[--optop];
        
        expr *tmp = get_op(op);
        outstack[outtop++] = tmp;
    }

    opstack[optop++] = nop;
}

static int is_op(int c) {
    return c == '+' ||
            c == '-' ||
            c == '*' ||
            c == '/' ||
            c == '^';
}

expr *eq_parse(const char *str) {
    outstack = malloc(sizeof(expr*) * strlen(str));
    outtop = 0;
    opstack = malloc(sizeof(int) * strlen(str));
    optop = 0;

    expr *out;

    int i = 0;
    while(str[i] != '\0') {
        if(is_num(str[i])) {
            float f = read_part(str, &i, 'i');
            if(str[i] == '\0') {
            }
            else if(str[i] == '.') {
                ++i;
                f += read_part(str, &i, 'f');
            }
            outstack[outtop++] = expr_new_const(f);
            --i;
        }
        else if(str[i] == '.') {
            ++i;
            float f = read_part(str, &i, 'f');
            outstack[outtop++] = expr_new_const(f);
            --i;
        }
        else if(str[i] == 'x') {
            outstack[outtop++] = expr_new_x();
        }
        else if(is_op(str[i])) {
            pop_ops(str[i]);
        }
        else if(str[i] == '(') {
            opstack[optop++] = '(';
        }
        else if(str[i] == ')') {
            while(opstack[optop - 1] != '(') {
                expr *tmp = get_op(opstack[--optop]);
                outstack[outtop++] = tmp;
            }
            --optop;
        }
        ++i;
    }

    while(optop >= 0) {
        expr *tmp = get_op(opstack[--optop]);
        outstack[outtop++] = tmp;
    }

    out = outstack[0];

    free(outstack);
    free(opstack);

    return out;
}
