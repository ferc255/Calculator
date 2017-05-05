#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>

#define TOKENS 11

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
	NON_TERM_S,
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
	.count = 7,
	.rules = (rule_t[])
	{
		{
			.left = NON_TERM_S,
			.rule_size = 1,
			.rule_elements = (rule_element_t[])
			{
				{NON_TERM, NON_TERM_E},
			},
		},
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
	if (token.item == NON_TERM_S)
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



typedef struct
{
	int size;
	int graph[TERM_COUNT][TERM_COUNT];
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

graph_t graph;
scheme_t scheme;

void add_points(int idx)
{
	bool added[TERM_COUNT];
	int i;
	for (i = 0; i < TERM_COUNT; i++)
	{
		added[i] = false;
	}

	for (i = 0; i < scheme.scheme[idx].size; i++)
	{
		point_t point = scheme.scheme[idx].list[i];
		if (point.position < grammar.rules[point.grammar_num].rule_size  &&
			grammar.rules[point.grammar_num].
			rule_elements[point.position].term_non_term	== NON_TERM)
		{
			item_t item = grammar.rules[point.grammar_num].
				rule_elements[point.position].item;
			if (!added[item])
			{
				added[item] = true;
				int j;
				for (j = 0; j < grammar.count; j++)
				{
					if (grammar.rules[j].left == item)
					{
						scheme.scheme[idx].list[scheme.scheme[idx].size]
							.grammar_num = j;
						scheme.scheme[idx].list[scheme.scheme[idx].size]
							.position = 0;
					
						scheme.scheme[idx].size++;
					}
				}
			}
		}
	}
}

bool is_equal_states(point_list_t a, point_list_t b)
{
	if (a.size != b.size)
	{
		return false;
	}

	int i;
	for (i = 0; i < a.size; i++)
	{
		bool found = false;
		int j;
		for (j = 0; j < b.size; j++)
		{
			if (a.list[i].grammar_num == b.list[j].grammar_num &&
				a.list[i].position == b.list[j].position)
			{
				found = true;
				break;
			}
		}
		if (!found)
		{
			return false;
		}
	}

	//printf(" YES ");
	return true;
}

void make_transition(int idx, item_t item)
{
	bool found = false;
	scheme.scheme[graph.size].size = 0;
	int i;
	for (i = 0; i < scheme.scheme[idx].size; i++)
	{
		point_t point = scheme.scheme[idx].list[i];
		if (point.position < grammar.rules[point.grammar_num].rule_size &&
			grammar.rules[point.grammar_num].rule_elements[point.position].
			item == item)
		{
			found = true;
			scheme.scheme[graph.size].list[scheme.scheme[graph.size].size]
				.grammar_num = point.grammar_num;
			scheme.scheme[graph.size].list[scheme.scheme[graph.size].size]
				.position = point.position + 1;
			scheme.scheme[graph.size].size++;
		}
	}

	if (!found)
	{
		return;
	}

	add_points(graph.size);
	found = false;
	for (i = 0; i < graph.size; i++)
	{
		if (is_equal_states(scheme.scheme[i], scheme.scheme[graph.size]))
		{
			found = true;
			graph.graph[idx][item] = i;
			break;
		}
	}

	if (!found)
	{
		graph.graph[idx][item] = graph.size;
		graph.size++;
	}

	//printf("%d", graph.graph[idx][item]);
}


void build_scheme()
{
	graph.size = 1;
	scheme.scheme[0].list[0].grammar_num = 0,
	scheme.scheme[0].list[0].position = 0;
	scheme.scheme[0].size = 1;
	add_points(0);
	int i;
	for (i = 0; i < graph.size; i++) //graph.size
	{
		int j;
		for (j = 0; j < TOKENS; j++)
		{
			//printf("%d %d: ", i, j);
	  		make_transition(i, j);
			//printf("\n");
			
		}
	}
	//make_transition(0, 7);
	//make_transition(0, 8);
	//make_transition(0, 9);
	//make_transition(0, 3);
	//make_transition(4, 8);
	
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

	build_scheme();
	printf("%d\n", graph.size);

	for (i = 0; i < graph.size; i++)
	{
		int j;
		for (j = 0; j < TOKENS; j++)
		{
			printf("%d ", graph.graph[i][j]);
		}
		printf("\n");
	}
	
	int y = 12;
	int j;
	for (j = 0; j < y; j++)
	{
		int z;
		for (z = 0; z < y; z++)
		{
			printf("%d,", j);
		}
		printf("\n");
		
		for (i = 0; i < scheme.scheme[j].size; i++)
		{
			printf("%d %d\n", scheme.scheme[j].list[i].grammar_num,
				   scheme.scheme[j].list[i].position);
		}
		printf("\n");
	}

	/*
	for (i = 0; i < TOKENS; i++)
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
	}*/









	

	return 0;
}
