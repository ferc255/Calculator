#ifndef _VALUES_H_
#define _VALUES_H_

typedef enum
{
	TERM_EMPTY,
	TERM_END,
	TERM_ID,
	TERM_LPAREN,
	TERM_RPAREN,
	TERM_PLUS,
	TERM_MUL,
	NON_TERM_E, // 7
	NON_TERM_T, 
	NON_TERM_F, // 9
	NON_TERM_S,
	TOKENS,
} item_t;

typedef enum
{
	AC_SHIFT,
	AC_REDUCE,
	AC_ACCEPT,
	AC_ERROR,
	AC_GOTO,
} action_t;

typedef struct {
  action_t action;
  int num;
} table_t;

typedef struct
{
  item_t token;
  int number;
} state_t;

typedef struct
{
	table_t** trans;
	item_t* grammar_left;
	int* grammar_size;
} tables_t;

#define TERM_COUNT (1 << CHAR_BIT)

typedef enum
{
	TERM,
	NON_TERM,
} term_non_term_t;


typedef struct rule_element_t
{
	term_non_term_t term_non_term;
	item_t item;
} rule_element_t;

typedef struct rule_t {
	item_t left;
	int rule_size;
	rule_element_t * rule_elements;
} rule_t;

typedef struct grammar_t
{
	int count;
	rule_t * rules;
} grammar_t;


typedef struct set_t
{
	int list_size;
	item_t list[TERM_COUNT];
	bool mask[TERM_COUNT];
} set_t;

typedef struct first_t
{
	set_t first[TERM_COUNT];
} first_t;

typedef struct follow_t
{
	set_t follow[TERM_COUNT];
} follow_t;

typedef enum
{
	NOT_COUNTED,
	IN_PROGRESS,
	COUNTED,
} rec_state_t;



typedef int state_id_t;

typedef struct
{
	int size;
	state_id_t graph[TERM_COUNT][TERM_COUNT];
} graph_t;

typedef struct
{
	int grammar_num;
	int position;
} point_t;

typedef struct
{
	int size;
	point_t list[TERM_COUNT];
} point_list_t;

typedef struct
{
	point_list_t scheme[TERM_COUNT];
} scheme_t;


typedef struct
{
	int size;
	table_t action[TERM_COUNT][TERM_COUNT];
} action_table_t;

typedef struct
{
	int size;
	state_id_t goto_table[TERM_COUNT][TERM_COUNT];
} goto_table_t;


#define YYSTYPE int
#define YYTOKENTYPE token_t

#endif /* _VALUES_H_ */
