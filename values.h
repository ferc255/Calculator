#ifndef _VALUES_H_
#define _VALUES_H_

#define INVALID_TOKEN -1
#define MAX_STATES 300
#define MAX_TOKENS 300

typedef int token_id_t;

typedef enum
{
    AC_SHIFT,
    AC_REDUCE,
    AC_ACCEPT,
    AC_ERROR,
} action_t;

typedef struct table_cell_t
{
    action_t action;
    int num;
} table_cell_t;

typedef struct transit_t
{
    int size;
    table_cell_t list[MAX_STATES][MAX_TOKENS];
} transit_t;

typedef struct item_t
{
    int prod;
    int pos;
    int end;
} item_t;

typedef struct item_list_t
{
    int size;
    item_t list[MAX_STATES];
} item_list_t;

typedef struct first_t
{
    int size;
    int* list;
} first_t;

typedef struct production_t
{
    int left;
    int* list;
    int size;
} production_t;

typedef struct token_names_t
{
    int size;
    char** list;
} token_names_t;

typedef struct grammar_t
{
    token_names_t token_names;
    production_t* prod;
    int size;
} grammar_t;

typedef struct token_t
{
    int id;
    int data;
} token_t;

typedef struct tables_t
{
    int* grammar_left;
    int* grammar_size;
    table_cell_t** trans;
} tables_t;

#endif /* _VALUES_H_ */
