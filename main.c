#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "values.h"
#include "my_yylex.h"

void f(int s)
{
    printf("fuck %d\n", s);
}
/*
void apply_prod_0(int* result, int stack_top) {}
void apply_prod_1(int* result, int stack_top)
{
    result[stack_top - 2] = result[stack_top - 2] + result[stack_top - 0];
}
void apply_prod_2(int* result, int stack_top)
{
    result[stack_top - 2] = result[stack_top - 2] - result[stack_top - 0];
}
void apply_prod_3(int* result, int stack_top)
{
    result[stack_top - 0] = result[stack_top - 0];
}

void (*apply[])() = 
{
    apply_prod_0,
    apply_prod_1,
    apply_prod_2,
    apply_prod_3,
};
*/
#include "executions.h"

token_t my_yylex1()
{
    token_t* a = (token_t[])
    {
        {4, 2},
        {5, 0},
        {4, 3},
        {5, 0},
        {4, 1},
        {5, 0},
        {4, 10},
        {3, 0},
        {4, 3},
        {0, 0},
    };
    static int i = 0;
    return a[i++];
    //token_t a = {.id=2, .data=2 };
    //return a;
}

void pr_re(int* re)
{
    int i;
    for (i = 0; i < 10; i++)
    {
        printf("%d ", re[i]);
    }
    printf("\n");
}

bool parse(tables_t* tables)
{
    int state[MAX_STATES] = {0};
    int result[MAX_STATES] = {0};
    int stack_top = 0;
    
    token_t token = my_yylex();
    //printf("%d %d\n", token.id, token.data);
    
    //printf("Rightmost derivaton reversed: ");
    
    while (true)
    {
        if (token.id == INVALID_TOKEN)
        {
            printf("Invalid token has been found at %d-th position of input.",
                token.data);
            return false;
        }
        
        int cur_state = state[stack_top];
        table_cell_t cell = tables->trans[cur_state][token.id];
        
        switch (cell.action)
        {
            case AC_SHIFT:
                stack_top++;
                state[stack_top] = cell.num;
                result[stack_top] = token.data;
                token = my_yylex();
                //printf("%d %d\n", token.id, token.data);
                break;
            case AC_REDUCE:
                apply[cell.num](&result, stack_top);
                //printf("%d ", cell.num);
                stack_top -= tables->grammar_size[cell.num];
                cur_state = state[stack_top];
                state[++stack_top] = tables->
                    trans[cur_state][tables->grammar_left[cell.num]].num;
                break;
            case AC_ACCEPT:
                //printf("\n");
            
                printf("Result: %d\n", result[1]);
                return true;
            case AC_ERROR:
                printf("Invalid token [id=%d] for the %d-th state.\n",
                    token.id, cur_state);
                return false;
        }
    }
}

int main()
{
    tables_t tables = 
    {
        #include "syn_tables.h"
        /*
        .grammar_left = (int[])
        {
            1, 2, 2, 2,
        },
        .grammar_size = (int[])
        {
            1, 3, 3, 1,
        },
        .trans = (table_cell_t*[])
        {
            (table_cell_t[])
            {
                {AC_ERROR, 0},
                {AC_ERROR, 0},
                {AC_SHIFT, 1},
                {AC_ERROR, 0},
                {AC_SHIFT, 6},
                {AC_ERROR, 0},
            },
            (table_cell_t[])
            {
                {AC_ACCEPT, 0},
                {AC_ERROR, 0},
                {AC_ERROR, 0},
                {AC_SHIFT, 2},
                {AC_ERROR, 0},
                {AC_SHIFT, 3},
            },
            (table_cell_t[])
            {
                {AC_ERROR, 0},
                {AC_ERROR, 0},
                {AC_ERROR, 0},
                {AC_ERROR, 0},
                {AC_SHIFT, 4},
                {AC_ERROR, 0},
            },
            (table_cell_t[])
            {
                {AC_ERROR, 0},
                {AC_ERROR, 0},
                {AC_ERROR, 0},
                {AC_ERROR, 0},
                {AC_SHIFT, 5},
                {AC_ERROR, 0},
            },
            (table_cell_t[])
            {
                {AC_REDUCE, 1},
                {AC_ERROR, 0},
                {AC_ERROR, 0},
                {AC_REDUCE, 1},
                {AC_ERROR, 0},
                {AC_REDUCE, 1},
            },
            (table_cell_t[])
            {
                {AC_REDUCE, 2},
                {AC_ERROR, 0},
                {AC_ERROR, 0},
                {AC_REDUCE, 2},
                {AC_ERROR, 0},
                {AC_REDUCE, 2},
            },
            (table_cell_t[])
            {
                {AC_REDUCE, 3},
                {AC_ERROR, 0},
                {AC_ERROR, 0},
                {AC_REDUCE, 3},
                {AC_ERROR, 0},
                {AC_REDUCE, 3},
            },
        },
        */
    };
    
    //printf("%s\n", tables.token_names.list[0]);
    //printf("%d\n", tables.trans[0][0].action);
    
    /*
    void (*a)() = &f;
    a(2);
    void (*fun[])() = 
    {
        f, f, f
    };
    */
    
    if (!parse(&tables))
    {
        return (EXIT_FAILURE);
    }
    
    return (EXIT_SUCCESS);
}