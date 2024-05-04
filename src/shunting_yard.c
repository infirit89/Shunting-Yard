//
// Created by georg on 5/4/2024.
//

#include "shunting_yard.h"

#include <stdint.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <sc/sc_map.h>
#include <sc/sc_queue.h>

struct operator
{
    uint8_t precedence;
    uint8_t symbol_count;
};

enum symbol_type
{
    none = 0,
    literal,
    operator,
    open_parentheses,
    closed_parentheses
};

struct symbol
{
    char value;
    enum symbol_type type;
    struct operator op;

};

/*
    Parenthesis    6
    Exponent       5
    Multiplication 4
    Division       3
    Addition       2
    Subtraction    1
*/

static struct operator operator_sub = { 1, 2 };
static struct operator operator_add = { 2, 2 };
static struct operator operator_div = { 3, 2 };
static struct operator operator_mul = { 4, 2 };

sc_queue_def(struct symbol, sym);

static struct sc_map_intv operator_map;

void sy_initialize()
{
    sc_map_init_intv(&operator_map, 0, 0);

    sc_map_put_intv(&operator_map, '-', &operator_sub);
    sc_map_put_intv(&operator_map, '+', &operator_add);
    sc_map_put_intv(&operator_map, '/', &operator_div);
    sc_map_put_intv(&operator_map, '*', &operator_mul);
}

double sy_solve(const char* expression)
{
    struct sc_queue_sym output_queue;
    sc_queue_init(&output_queue);
    struct sc_queue_sym holding_queue;
    sc_queue_init(&holding_queue);

    bool error = false;

    size_t expression_size = strlen(expression);

    for (int i = 0; i < expression_size; ++i)
    {
        if(expression[i] == ' ')
            continue;

        if(isdigit(expression[i]))
        {
            struct symbol symbol = { expression[i], literal };
            sc_queue_add_last(&output_queue, symbol);
        }
        else
        {
            void* out = sc_map_get_intv(&operator_map, expression[i]);
            if(sc_map_found(&operator_map))
            {
                struct operator op = *(struct operator*)out;
                while(!sc_queue_empty(&holding_queue))
                {
                    struct symbol front_symbol = sc_queue_at(&holding_queue, sc_queue_last(&holding_queue));

                    if(front_symbol.type != operator)
                        continue;

                    if(front_symbol.op.precedence >= op.precedence)
                    {
                        sc_queue_add_last(&output_queue, front_symbol);
                        sc_queue_del_first(&holding_queue);
                    }
                    else
                        break;
                }

                struct symbol symbol = { expression[i], operator, op };
                sc_queue_add_first(&holding_queue, symbol);
            }
            else
            {
                printf("Error! Encountered an invalid symbol: %c", expression[i]);
                error = true;
                goto shutdown;
            }
        }
    }

    while(!sc_queue_empty(&holding_queue))
    {
        struct symbol front_symbol = sc_queue_at(&holding_queue, sc_queue_last(&holding_queue));
        sc_queue_add_last(&output_queue, front_symbol);
        sc_queue_del_first(&holding_queue);
    }

    printf("%s\n", expression);
    printf("RPN: ");
    struct symbol elem;
    sc_queue_foreach (&output_queue, elem) {
            printf("%c", elem.value);
        }
    printf("\n");

    struct sc_queue_double solution_queue;
    sc_queue_init(&solution_queue);

    sc_queue_foreach(&output_queue, elem)
    {
        switch (elem.type) {
            case literal:
            {
                sc_queue_add_last(&solution_queue, elem.value - '0');
                break;
            }
            case operator:
            {
                double* mem = malloc(elem.op.symbol_count * sizeof(double));

                for (uint8_t i = 0; i < elem.op.symbol_count; ++i) {
                    mem[i] = sc_queue_peek_last(&solution_queue);
                    sc_queue_del_last(&solution_queue);
                }
                switch (elem.value) {
                    case '*': sc_queue_add_first(&solution_queue, mem[1] * mem[0]); break;
                    case '+': sc_queue_add_first(&solution_queue, mem[1] + mem[0]); break;
                    case '-': sc_queue_add_first(&solution_queue, mem[1] - mem[0]); break;
                    case '/': sc_queue_add_first(&solution_queue, mem[1] / mem[0]); break;
                }

                free(mem);
                break;
            }
            case none:
            {
                printf("Error!");
                error = true;
                goto shutdown;
            }
        }
    }

    double result = error ? 0.0 : sc_queue_peek_first(&solution_queue);

    shutdown:
    sc_queue_term(&output_queue);
    sc_queue_term(&holding_queue);
    sc_queue_term(&solution_queue);

    return result;
}

void sy_shutdown()
{
    sc_map_term_intv(&operator_map);
}
