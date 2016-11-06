#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "expression.h"
#include "util/stack.h"

#include "equation.h"

typedef enum tokentype {
    CONSTANT,
    X,
    OPERATOR,
    FUNCTION,
    LEFT_PAREN,
    ARG_SEPARATOR,
    RIGHT_PAREN,
    INT_SEPARATOR,
    DX,
    END,
    UNDEFINED
} tokentype;

typedef enum operator {
    ADD,
    SUBTRACT,
    MULTIPLY,
    DIVIDE,
    EXPONENTIATE,
    PAREN,
    ARROW,
    INTEGRAL,
    SIN,
    COS,
    TAN,
    SEC,
    CSC,
    COT,
    ARCSIN,
    ARCCOS,
    ARCTAN,
    ARCSEC,
    ARCCSC,
    ARCCOT,
    FLOOR,
    CEIL,
    ROUND,
    ABS,
    LN,
    SQRT
} operator;

typedef struct token {
    tokentype type;
    void *data;
} token;

static stack_generate(exprs, expr*);
static stack_generate(ops, operator);

static int is_num(int c) {
    return c >= '0' && c <= '9';
}

static int is_letter(int c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
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

static int is_op(int c) {
    return c == '+' ||
            c == '-' ||
            c == '*' ||
            c == '/' ||
            c == '^';
}

static operator operator_from_char(char c) {
    switch(c) {
        case '+':
            return ADD;
        case '-':
            return SUBTRACT;
        case '*':
            return MULTIPLY;
        case '/':
            return DIVIDE;
        case '^':
            return EXPONENTIATE;
    }
}

//TODO: Better macro names
#define TDATA(name,type,index) (((type*)(name.data))[index])
#define DATA(type,index) TDATA(t,type,index)
#define INIT(tok,type,data) t = (token){ tok, malloc(sizeof(type)) }; DATA(type,0) = data
#define RESULT(tok, type, data) INIT(tok, type, data); goto end
#define SRESULT(tok) t = (token){ tok, NULL }; goto end

static int is_string_token(const char *str, const char *compare, int *index) {
    int i = *index;

    int j = 0;

    for(;;) {
        if(compare[j] == '\0') { break; }
        if(str[i] != compare[j]) {
            return 0;
        }
        ++i;
        ++j;
    }

    /* if(is_letter(str[i])) { return 0; } */ /* TODO: Figure out whether this is a good idea */

    *index = i - 1;
    return 1;
}

static token get_string_token(const char *str, int *index) {
    token t;

    int i = *index;

    if(is_string_token(str, "x", &i)) {
        SRESULT(X);
    }

    if(is_string_token(str, "e", &i)) {
        RESULT(CONSTANT, float, M_E);
    }

    if(is_string_token(str, "pi", &i)) {
        RESULT(CONSTANT, float, M_PI);
    }

    if(is_string_token(str, "dx", &i)) {
        SRESULT(DX);
    }

    if(is_string_token(str, "sin", &i)) {
        RESULT(FUNCTION, operator, SIN);
    }

    if(is_string_token(str, "cos", &i)) {
        RESULT(FUNCTION, operator, COS);
    }

    if(is_string_token(str, "tan", &i)) {
        RESULT(FUNCTION, operator, TAN);
    }

    if(is_string_token(str, "sec", &i)) {
        RESULT(FUNCTION, operator, SEC);
    }

    if(is_string_token(str, "csc", &i)) {
        RESULT(FUNCTION, operator, CSC);
    }

    if(is_string_token(str, "cot", &i)) {
        RESULT(FUNCTION, operator, COT);
    }

    if(is_string_token(str, "arcsin", &i)) {
        RESULT(FUNCTION, operator, ARCSIN);
    }

    if(is_string_token(str, "arccos", &i)) {
        RESULT(FUNCTION, operator, ARCCOS);
    }

    if(is_string_token(str, "arctan", &i)) {
        RESULT(FUNCTION, operator, ARCTAN);
    }

    if(is_string_token(str, "arcsec", &i)) {
        RESULT(FUNCTION, operator, ARCSEC);
    }

    if(is_string_token(str, "arccsc", &i)) { /* TODO: Better parsing of this mess? */
        RESULT(FUNCTION, operator, ARCCSC);
    }

    if(is_string_token(str, "arccot", &i)) {
        RESULT(FUNCTION, operator, ARCCOT);
    }

    if(is_string_token(str, "floor", &i)) {
        RESULT(FUNCTION, operator, FLOOR);
    }

    if(is_string_token(str, "ceil", &i)) {
        RESULT(FUNCTION, operator, CEIL);
    }

    if(is_string_token(str, "round", &i)) {
        RESULT(FUNCTION, operator, ROUND);
    }

    if(is_string_token(str, "abs", &i)) {
        RESULT(FUNCTION, operator, ABS);
    }

    if(is_string_token(str, "ln", &i)) {
        RESULT(FUNCTION, operator, LN);
    }

    if(is_string_token(str, "sqrt", &i)) {
        RESULT(FUNCTION, operator, SQRT);
    }

    if(is_string_token(str, "int", &i)) {
        RESULT(FUNCTION, operator, INTEGRAL);
    }

    SRESULT(UNDEFINED);

    end:

    *index = i;
    return t;
}

static token get_token(const char *str, int *index, tokentype last) {
    token t;

    int i = *index;

    while(str[i] == ' ') { i++; };
    if(str[i] == '\0') {
        SRESULT(END);
    }

    if(is_letter(str[i])) {
        t = get_string_token(str, &i);

        goto end;
    }

    if(last != X && last != CONSTANT) {
        if(str[i] == '-') {
            if(is_num(str[i + 1]) || str[i + 1] == '.') {
                ++i;
                t = get_token(str, &i, last); /* Read num part */
                DATA(float, 0) *= -1; /* Negate */

                goto end;
            }
            /* TODO: Implement -x -> (-1 * x) */
        }
    }

    if(str[i] == ',') {
        SRESULT(ARG_SEPARATOR);
    }

    if(str[i] == ':') {
        SRESULT(INT_SEPARATOR);
    }

    if(str[i] == '(') {
        RESULT(LEFT_PAREN, operator, PAREN);
    }

    if(str[i] == ')') {
        SRESULT(RIGHT_PAREN);
    }

    if(is_num(str[i])) {
        float f = read_part(str, &i, 'i'); /* Read int part */

        if(str[i] == '.') { /* Read float part if decimal exists */
            ++i;
            f += read_part(str, &i, 'f');
        }
        --i;

        RESULT(CONSTANT, float, f);
    }

    if(str[i] == '.') {
        ++i;
        if(!is_num(str[i])) { /* If this part isn't a number, this is just a floating decimal point, which is meaningless */
            SRESULT(UNDEFINED);
        }
        float f = read_part(str, &i, 'f');
        --i;

        RESULT(CONSTANT, float, f);
    }

    if(is_op(str[i])) { //TODO: Efficient way to read operators
        if(str[i + 1] == '>') {
            ++i;
            RESULT(OPERATOR, operator, ARROW);
        }
        else {
            RESULT(OPERATOR, operator, operator_from_char(str[i]));
        }
    }

    SRESULT(UNDEFINED);

    end:

    *index = i;
    return t;
}

static int precedence(operator op) {
    switch(op) {
        case MULTIPLY:
        case DIVIDE:
        return 1;

        case EXPONENTIATE:
        return 0;

        case ARROW:
        return 3;
        
        case ADD:
        case SUBTRACT:
        default:
        return 2;
    }
}

static int is_right(operator op) {
    switch(op) {
        case EXPONENTIATE:
        return 1;

        case ADD:
        case SUBTRACT:
        case MULTIPLY:
        case DIVIDE:
        default:
        return 0;
    }
}

static int should_pop(operator op1, operator op2) {
    if(op2 == PAREN) { return 0; }
    if(op1 >= INTEGRAL || op2 >= INTEGRAL) { return 0; }
    if(is_right(op2)) {
        return precedence(op2) < precedence(op1);
    }
    return precedence(op2) <= precedence(op1);
}

static int exprs_has_args(int amount) {
    return exprs->top >= amount;
}

static expr *get_op(operator op) {
    if(!exprs_has_args(2)) {
        return NULL; /* Return null when not enough arguments for operator */
    }
    switch(op) {
        case ADD:
            return expr_new_add(exprs_pop(), exprs_pop());
        case SUBTRACT:
            return expr_new_sub(exprs_pop(), exprs_pop());
        case MULTIPLY:
            return expr_new_mul(exprs_pop(), exprs_pop());
        case DIVIDE:
            return expr_new_div(exprs_pop(), exprs_pop());
        case EXPONENTIATE:
            return expr_new_pow(exprs_pop(), exprs_pop());
        case ARROW:
            return expr_new_range(exprs_pop(), exprs_pop());
    }
    return NULL; /* Another problem occured */
}

/* Returns 0 if some sort of error occured */
static int pop_ops(operator nop) {
    operator op;

    while(ops_can_pop()) {
        if(!should_pop(nop, ops_read(ops->top - 1))) { break; }
        op = ops_pop();
        
        expr *tmp = get_op(op);
        if(tmp == NULL) { /* NULL represents some sort of problem, return 0 */
            return 0;
        }
        exprs_push(tmp);
    }

    ops_push(nop);
    return 1;
}

/* Returns 0 if multiplication should be done, according to syntax, but fails */
static int checkmul(tokentype next, tokentype prev) {
    if(next == OPERATOR || prev == OPERATOR
    || next == UNDEFINED || prev == UNDEFINED
    || next == INT_SEPARATOR || prev == INT_SEPARATOR
    || next == DX || prev == DX) { return 1; }

    if(prev == LEFT_PAREN || next == RIGHT_PAREN) { return 1; }

    if(prev == FUNCTION) { return 1; }

    if(next != prev) {
        return pop_ops(MULTIPLY);
    }

    return 1;
}

static int parse_back(operator begin, int close) {
    int mismatched = 1;
    while(ops_can_pop()) {
        if(ops_read(ops->top - 1) == begin) {
            mismatched = 0;
            break;
        }
        
        expr *tmp = get_op(ops_pop());
        if(tmp == NULL) {
            return 0;
        }
        exprs_push(tmp);
    }
    if(mismatched) { /* There was no left parentheses! */
        return 0;
    }

    if(close) {
        ops_pop();
    }

    return 1;
}

static int readfunc() {
    if(ops_can_pop()) {
        if(ops_read(ops->top - 1) >= SIN) { /* TODO: More organized */
            if(!exprs_has_args(1)) {
                return 0;
            }
            switch(ops_pop()) {
                case SIN:
                    exprs_push(expr_new_sin(exprs_pop()));
                    break;
                case COS:
                    exprs_push(expr_new_cos(exprs_pop()));
                    break;
                case TAN:
                    exprs_push(expr_new_tan(exprs_pop()));
                    break;
                case SEC:
                    exprs_push(expr_new_sec(exprs_pop()));
                    break;
                case CSC:
                    exprs_push(expr_new_csc(exprs_pop()));
                    break;
                case COT:
                    exprs_push(expr_new_cot(exprs_pop()));
                    break;
                case ARCSIN:
                    exprs_push(expr_new_arcsin(exprs_pop()));
                    break;
                case ARCCOS:
                    exprs_push(expr_new_arccos(exprs_pop()));
                    break;
                case ARCTAN:
                    exprs_push(expr_new_arctan(exprs_pop()));
                    break;
                case ARCSEC:
                    exprs_push(expr_new_arcsec(exprs_pop()));
                    break;
                case ARCCSC:
                    exprs_push(expr_new_arccsc(exprs_pop()));
                    break;
                case ARCCOT:
                    exprs_push(expr_new_arccot(exprs_pop()));
                    break;
                case FLOOR:
                    exprs_push(expr_new_floor(exprs_pop()));
                    break;
                case CEIL:
                    exprs_push(expr_new_ceil(exprs_pop()));
                    break;
                case ROUND:
                    exprs_push(expr_new_round(exprs_pop()));
                    break;
                case ABS:
                    exprs_push(expr_new_abs(exprs_pop()));
                    break;
                case LN:
                    exprs_push(expr_new_ln(exprs_pop()));
                    break;
                case SQRT:
                    exprs_push(expr_new_sqrt(exprs_pop()));
                    break;
            }
        }
    }
    return 1;
}

static int handle_token(token tok) {
    switch(tok.type) {
        case CONSTANT:
            exprs_push(expr_new_const(TDATA(tok, float, 0)));
            break;
        case X:
            exprs_push(expr_new_x());
            break;
        case LEFT_PAREN:
            ops_push(PAREN);
            break;
        case FUNCTION:
        case OPERATOR:
            if(!pop_ops(TDATA(tok,operator,0))) {
                return 0;
            }
            
            break;
        case INT_SEPARATOR:
            if(!parse_back(INTEGRAL, 0)) {
                return 0;
            }
            break;
        case DX:
            if(!parse_back(INTEGRAL, 1)) {
                return 0;
            }

            if(!exprs_has_args(2)) { /* Not enough arguments for integral */
                return 0;
            }

            exprs_push(expr_new_int(exprs_pop(), exprs_pop()));
            break;
        case RIGHT_PAREN:
            if(!parse_back(PAREN, 1)) {
                return 0;
            }

            if(!readfunc()) {
                return 0;
            }
            
            break;
        default:
        case UNDEFINED: //TODO: Add error message
            return 0;
    }
    return 1;
}

expr *eq_parse(const char *str) {
    exprs_init(64); /* 64 tokens is a reasonable starting amount */
    ops_init(64);

    expr *out = NULL;

    int i = 0;

    token tok;
    tokentype lasttype = UNDEFINED;

    while(str[i] != '\0') {
        tok = get_token(str, &i, lasttype);

        /* Generate multiplier token if needed */
        if(!checkmul(tok.type, lasttype)) {
            goto cleanup;
        }

        /* Handle current token */
        if(!handle_token(tok)) {
            goto cleanup;
        }

        /* Get rid of allocated data */
        if(tok.data != NULL) { free(tok.data); }

        /* Next loop */
        ++i;
        lasttype = tok.type;
    }

    while(ops_can_pop()) {
        if(ops_read(ops->top - 1) == PAREN) { goto cleanup; } /* There shouldn't be any unevaluated parens */
        expr *tmp = get_op(ops_pop());
        if(tmp == NULL) {
            goto cleanup;
        }
        exprs_push(tmp);
    }

    out = exprs_read(0);

    /* Cleanup section - if there is a failure, these free's need to be done */

    cleanup:

    exprs_free();
    ops_free();

    return out;
}
