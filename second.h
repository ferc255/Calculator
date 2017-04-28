#define N 12

#define TERM_COUNT (1 << CHAR_BIT)

typedef enum
{
	TERM,
	NON_TERM,
} term_non_term_t;

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
	NON_TERM_E1,
	NON_TERM_T, 
	NON_TERM_T1,
	NON_TERM_F, // 11
} item_t;

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



grammar_t grammar =
{
	.count = 8,
	.rules = (rule_t[])
	{
		{
			.left = NON_TERM_E,
			.rule_size = 2,
			.rule_elements = (rule_element_t[])
			{
				{NON_TERM, NON_TERM_T},
				{NON_TERM, NON_TERM_E1},
			},
		},
		{
			.left = NON_TERM_E1,
			.rule_size = 3,
			.rule_elements = (rule_element_t[])
			{
				{TERM, TERM_PLUS},
				{NON_TERM, NON_TERM_T},
				{NON_TERM, NON_TERM_E1},
			},
		},
		{
			.left = NON_TERM_E1,
			.rule_size = 1,
			.rule_elements = (rule_element_t[])
			{
				{TERM, TERM_EMPTY},
			},
		},
		{
			.left = NON_TERM_T,
			.rule_size = 2,
			.rule_elements = (rule_element_t[])
			{
				{NON_TERM, NON_TERM_F},
				{NON_TERM, NON_TERM_T1},
			},
		},
		{
			.left = NON_TERM_T1,
			.rule_size = 3,
			.rule_elements = (rule_element_t[])
			{
				{TERM, TERM_MUL},
				{NON_TERM, NON_TERM_F},
				{NON_TERM, NON_TERM_T1},
			},
		},
		{
			.left = NON_TERM_T1,
			.rule_size = 1,
			.rule_elements = (rule_element_t[])
			{
				{TERM, TERM_EMPTY},
			},
		},
		{
			.left = NON_TERM_F,
			.rule_size = 3,
			.rule_elements = (rule_element_t[])
			{
				{TERM, TERM_LPAREN},
				{NON_TERM_E, NON_TERM_E},
				{TERM, TERM_RPAREN},
			},
		},
		{
			.left = NON_TERM_F,
			.rule_size = 1,
			.rule_elements = (rule_element_t[])
			{
				{TERM, TERM_ID},
			},
		},
	}
};

first_t first;
follow_t follow;
rec_state_t used[TERM_COUNT];


