#ifndef _VALUES_H_
#define _VALUES_H_

#define INVALID_TOKEN -1
#define MAX_STATES 300

typedef enum
{
    AC_SHIFT,
    AC_REDUCE,
    AC_ACCEPT,
    AC_ERROR,
} action_t;


typedef struct token_t
{
    int id;
    int data;
} token_t;

typedef struct token_names_t
{
    int size;
    char** list;
} token_names_t;

typedef struct table_cell_t
{
    action_t action;
    int num;
} table_cell_t;

typedef struct tables_t
{
    token_names_t token_names;
    int* grammar_left;
    int* grammar_size;
    table_cell_t** trans;
} tables_t;

#endif /* _VALUES_H_ */
