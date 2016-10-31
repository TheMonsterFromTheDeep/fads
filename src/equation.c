#include <string.h>
#include <stdlib.h>
#include "expression.h"
#include "util/stack.h"

#include "equation.h"

//static expr **outstack;
//static int outtop;
//static int *opstack;
//static int optop;

static stack_generate(exprs, expr*);
static stack_generate(ops, int);

typedef enum token {
    NUMBER,
    XVAL,
    PARENS_LEFT,
    PARENS_RIGHT,
    OPERATOR,
    MISC
} token;

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

        case '>':
        return 3;
        
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
            return expr_new_add(exprs_pop(), exprs_pop());
        case '-':
            return expr_new_sub(exprs_pop(), exprs_pop());
        case '*':
            return expr_new_mul(exprs_pop(), exprs_pop());
        case '/':
            return expr_new_div(exprs_pop(), exprs_pop());
        case '^':
            return expr_new_pow(exprs_pop(), exprs_pop());
        case '>':
            return expr_new_range(exprs_pop(), exprs_pop());
        case 'i':
            return expr_new_int(exprs_pop(), exprs_pop());
    }
    return NULL;
}

static void pop_ops(int nop) {
    int op;

    while(ops_can_pop()) {
        if(!should_pop(nop, ops_read(ops->top - 1))) { break; }
        op = ops_pop();
        
        expr *tmp = get_op(op);
        exprs_push(tmp);
    }

    ops_push(nop);
}

static int is_op(int c) {
    return c == '+' ||
            c == '-' ||
            c == '*' ||
            c == '/' ||
            c == 'i' ||
            c == '^';
}

static void checkmul(token next, token prev) {
    if(next == OPERATOR || prev == OPERATOR
    || next == MISC || prev == MISC) { return; }

    if(prev == PARENS_LEFT || next == PARENS_RIGHT) { return; }

    if(next != prev) {
        pop_ops('*');
    }
}

expr *eq_parse(const char *str) {
    exprs_init(64);
    ops_init(64);

    expr *out;

    int i = 0;
    token previous = MISC;
    token next;
    while(str[i] != '\0') {
        if(is_num(str[i])) {
            checkmul(next = NUMBER, previous);
            float f = read_part(str, &i, 'i');
            if(str[i] == '\0') {
            }
            else if(str[i] == '.') {
                ++i;
                f += read_part(str, &i, 'f');
            }
            exprs_push(expr_new_const(f));
            --i;
            
        }
        else if(str[i] == '.') {
            checkmul(next = NUMBER, previous);
            ++i;
            float f = read_part(str, &i, 'f');
            exprs_push(expr_new_const(f));
            --i;
        }
        else if(str[i] == 'x') {
            checkmul(next = XVAL, previous);
            exprs_push(expr_new_x());
        }
        else if(is_op(str[i])) {
            checkmul(next = OPERATOR, previous);
            if(str[i] == '-' && str[i + 1] == '>') {
                ++i;
                pop_ops('>');
            }
            else { pop_ops(str[i]); }
        }
        else if(str[i] == '(') {
            checkmul(next = PARENS_LEFT, previous);
            ops_push('(');
        }
        else if(str[i] == ')') {
            checkmul(next = PARENS_RIGHT, previous);
            while(ops_can_pop()) {
                if(ops_read(ops->top - 1) == '(') { break; }
                expr *tmp = get_op(ops_pop());
                exprs_push(tmp);
            }
            ops_pop();
        }
        ++i;
        previous = next;
    }

    while(ops_can_pop()) {
        if(ops_read(ops->top - 1) == ')') { break; } /* Abandon ship! (TODO: error handling) */
        expr *tmp = get_op(ops_pop());
        exprs_push(tmp);
    }

    out = exprs_read(0);

    exprs_free();
    ops_free();

    return out;
}
