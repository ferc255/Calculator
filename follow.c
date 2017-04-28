#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>

#define N 10

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
	NON_TERM_T, 
	NON_TERM_F, // 9
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
	.count = 6,
	.rules = (rule_t[])
	{
		{
			.left = NON_TERM_E,
			.rule_size = 3,
			.rule_elements = (rule_element_t[])
			{
				{NON_TERM, NON_TERM_E},
				{TERM, TERM_PLUS},
				{NON_TERM, NON_TERM_T},
			},
		},
		{
			.left = NON_TERM_E,
			.rule_size = 1,
			.rule_elements = (rule_element_t[])
			{
				{NON_TERM, NON_TERM_T},
			},
		},
		{
			.left = NON_TERM_T,
			.rule_size = 3,
			.rule_elements = (rule_element_t[])
			{
				{NON_TERM, NON_TERM_T},
				{TERM, TERM_MUL},
				{NON_TERM, NON_TERM_F},
			},
		},
		{
			.left = NON_TERM_T,
			.rule_size = 1,
			.rule_elements = (rule_element_t[])
			{
				{NON_TERM, NON_TERM_F},
			},
		},
		{
			.left = NON_TERM_F,
			.rule_size = 3,
			.rule_elements = (rule_element_t[])
			{
				{TERM, TERM_LPAREN},
				{NON_TERM, NON_TERM_E},
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


bool contains(char arr[N][N], int size, char* token)
{
	int i;
	for (i = 0; i < size; i++)
	{
		if (strcmp(arr[i], token) == 0)
		{
			return true;
		}
	}
	return false;
}

void calcFirst(rule_element_t token)
{
	if (used[token.item] == COUNTED)
	{
		return;
	}
	if (used[token.item] == IN_PROGRESS)
	{
		printf("Invalid grammar\n");
		exit(0);
	}
	if (token.term_non_term == TERM)
	{
		first.first[token.item].list[0] = token.item;
		first.first[token.item].mask[token.item] = true;
		first.first[token.item].list_size = 1;
		used[token.item] = COUNTED;
		return;
	}

	used[token.item] = IN_PROGRESS;
	int i;
	for (i = 0; i < grammar.count; i++)
	{
		if (grammar.rules[i].left == token.item)
		{
			bool found = false;
			int j;
			
			for (j = 0; j < grammar.rules[i].rule_size; j++)
			{
				if (grammar.rules[i].rule_elements[j].item == token.item)
				{
					found = true;
					break;
				}
				
				calcFirst(grammar.rules[i].rule_elements[j]);
				item_t neigh = grammar.rules[i].rule_elements[j].item;
				int z;
				for (z = 0; z < first.first[neigh].list_size; z++)
				{
					if (first.first[neigh].list[z] != TERM_EMPTY &&
						!first.first[token.item].
						mask[first.first[neigh].list[z]])
					{
						first.first[token.item].list[first.
							first[token.item].list_size] =
								first.first[neigh].list[z];
						first.first[token.item].mask[first.first[neigh].
							list[z]] = true;
						first.first[token.item].list_size++;
					}
				}

				if (!first.first[neigh].mask[TERM_EMPTY])
				{
					found = true;
					break;
				}
			}


			if (!found && !first.first[token.item].
				mask[TERM_EMPTY])
			{
				first.first[token.item].list[first.
					first[token.item].list_size] = TERM_EMPTY;
				first.first[token.item].mask[TERM_EMPTY] = true;
				first.first[token.item].list_size++;
			}				
		}
	}



	for (i = 0; i < grammar.count; i++)
	{
		if (grammar.rules[i].left == token.item)
		{
			int j;
			for (j = 0; j < grammar.rules[i].rule_size; j++)
			{
				if (grammar.rules[i].rule_elements[j].item != token.item)
				{
					calcFirst(grammar.rules[i].rule_elements[j]);
				}
				item_t neigh = grammar.rules[i].rule_elements[j].item;
				int z;
				for (z = 0; z < first.first[neigh].list_size; z++)
				{
					if (first.first[neigh].list[z] != TERM_EMPTY &&
						!first.first[token.item].
						mask[first.first[neigh].list[z]])
					{
						first.first[token.item].list[first.
							first[token.item].list_size] =
								first.first[neigh].list[z];
						first.first[token.item].mask[first.first[neigh].
							list[z]] = true;
						first.first[token.item].list_size++;
					}
				}

				if (!first.first[neigh].mask[TERM_EMPTY])
				{
					break;
				}
			}
		}
	}

	used[token.item] = COUNTED;
}


void calcFollow(rule_element_t token)
{
	if (used[token.item] == COUNTED)
	{
		return;
	}
	if (used[token.item] == IN_PROGRESS)
	{
		printf("Invalid grammar\n");
		exit(0);
	}
	if (token.item == NON_TERM_E)
	{
		follow.follow[token.item].list[0] = TERM_END;
		follow.follow[token.item].mask[TERM_END] = true;
		follow.follow[token.item].list_size = 1;
	}

	used[token.item] = IN_PROGRESS;
	int i;
	for (i = 0; i < grammar.count; i++)
	{
		int j;
		for (j = 0; j < grammar.rules[i].rule_size; j++)
		{
			if (grammar.rules[i].rule_elements[j].item == token.item)
			{
				bool found = false;
				int w = 0;
				for (w = j + 1; w < grammar.rules[i].rule_size; w++)
				{
					item_t neigh = grammar.rules[i].rule_elements[w].item;
					int z;
					for (z = 0; z < first.first[neigh].list_size; z++)
					{
						if (first.first[neigh].list[z] != TERM_EMPTY &&
							!follow.follow[token.item].
							mask[first.first[neigh].list[z]])
						{
							follow.follow[token.item].list[follow.
								follow[token.item].list_size] =
									first.first[neigh].list[z];
							follow.follow[token.item].mask[first.
								first[neigh].list[z]] = true;
							follow.follow[token.item].list_size++;
						}
					}

					if (!first.first[neigh].mask[TERM_EMPTY])
					{
						found = true;
						break;
					}							
				}

				if (!found && grammar.rules[i].left != token.item)
				{
					rule_element_t aux = {NON_TERM, grammar.rules[i].left};
					calcFollow(aux);
					item_t neigh = grammar.rules[i].left;
					int z;
					for (z = 0; z < follow.follow[neigh].list_size; z++)
					{
						if (!follow.follow[token.item].
							mask[follow.follow[neigh].list[z]])
						{
							follow.follow[token.item].list[follow.
								follow[token.item].list_size] =
									follow.follow[neigh].list[z];
							follow.follow[token.item].mask[follow.
								follow[neigh].list[z]] = true;
							follow.follow[token.item].list_size++;
						}
					}
				}
			}
		}
	}
	used[token.item] = COUNTED;
}


int main()
{	
	int i;
	for (i = 0; i < TERM_COUNT; i++)
	{
		first.first[i].list_size = follow.follow[i].list_size = 0;

		int j;
		for (j = 0; j < TERM_COUNT; j++)
		{
			first.first[i].mask[j] = follow.follow[i].mask[j] = false;
		}
	}
	for (i = 0; i < TERM_COUNT; i++)
	{
		used[i] = NOT_COUNTED;
	}
	for (i = 0; i < grammar.count; i++)
	{
		int j;
		for (j = 0; j < grammar.rules[i].rule_size; j++)
		{
			calcFirst(grammar.rules[i].rule_elements[j]);
		}
	}

	for (i = 0; i < TERM_COUNT; i++)
	{
		used[i] = NOT_COUNTED;
	}
	for (i = 0; i < grammar.count; i++)
	{
		int j;
		for (j = 0; j < grammar.rules[i].rule_size; j++)
		{
			calcFollow(grammar.rules[i].rule_elements[j]);
		}
	}
	
	for (i = 0; i < N; i++)
	{
		printf("%d\nFIRST: ", i);
		int j;
		for (j = 0; j < first.first[i].list_size; j++)
		{
			printf("%d ", first.first[i].list[j]);
		}
		
		printf("\nFOLLOW: ");
		for (j = 0; j < follow.follow[i].list_size; j++)
		{
			printf("%d ", follow.follow[i].list[j]);
		}

		printf("\n\n");
	}

	return 0;
}
