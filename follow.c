#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>

#include "values.h"


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


void merge_sets (set_t * left, set_t * right)
{
	int i;
	for (i = 0; i < right->list_size; i++)
	{
		if (right->list[i] != TERM_EMPTY &&
			!left->mask[right->list[i]])
		{
			left->list[left->list_size++] = right->list[i];
			left->mask[right->list[i]] = true;
		}
	}
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
				merge_sets (&first.first[token.item], &first.first[neigh]);

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
					first[token.item].list_size++] = TERM_EMPTY;
				first.first[token.item].mask[TERM_EMPTY] = true;
			}				
		}
	}

	if (first.first[token.item].mask[TERM_EMPTY])
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
					merge_sets (&first.first[token.item], &first.first[neigh]);
	
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
		bool contain_self = false;
		int j;
		for (j = 0; j < grammar.rules[i].rule_size; j++)
		{
			if (grammar.rules[i].rule_elements[j].item == token.item)
			{
				contain_self = true;
				break;
			}
		}
		
		bool has_nonempty = false;
		for (j = j + 1; j < grammar.rules[i].rule_size; j++)
		{
			item_t neigh = grammar.rules[i].rule_elements[j].item;
			merge_sets(&follow.follow[token.item], &first.first[neigh]);

			if (!first.first[neigh].mask[TERM_EMPTY])
			{
				has_nonempty = true;
				break;
			}
		}

		if (contain_self && !has_nonempty && grammar.rules[i].left != token.item)
		{
			rule_element_t left = {NON_TERM, grammar.rules[i].left};
			calcFollow(left);
			item_t neigh = grammar.rules[i].left;
			merge_sets(&follow.follow[token.item], &follow.follow[neigh]);
		}
	}
	used[token.item] = COUNTED;
}





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
	// qsort (array, count, elem_size, comparator);
}

bool is_equal_states(point_list_t a, point_list_t b)
{
	if (a.size != b.size)
	{
		return false;
	}
	// linear compare of sorted arrays
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
	
}

void build_automaton(action_table_t* action_table, goto_table_t* goto_table)
{
	action_table->size = graph.size;
	
	int i;
	for (i = 0; i < graph.size; i++)
	{
		int j;
		for (j = 0; j < TOKENS; j++)
		{
			if (graph.graph[i][j] == 0)
			{
				action_table->action[i][j].action = AC_ERROR;
			}
			else
			{
				action_table->action[i][j].action = AC_GOTO;
				action_table->action[i][j].num = graph.graph[i][j];
			}
		}
	}
	
	
	
	for (i = 0; i < graph.size; i++)
	{
		int j;
		for (j = 0; j < scheme.scheme[i].size; j++)
		{
			point_t* point = &scheme.scheme[i].list[j];
			rule_t* rule = &grammar.rules[point->grammar_num];
			if (point->position == rule->rule_size)
			{
				if (point->grammar_num == 0)
				{
					action_table->action[i][TERM_END].action = AC_ACCEPT;
				}
				else
				{
					int k;
					for (k = 0; k < follow.follow[rule->left].list_size; k++)
					{
						item_t item = follow.follow[rule->left].list[k];
						action_table->action[i][item].action = AC_REDUCE;
						action_table->action[i][item].num = point->grammar_num;
					}
				}
			}
			else
			{
				item_t item = grammar.rules[point->grammar_num].rule_elements[point->position].item;
				action_table->action[i][item].action = AC_SHIFT; // It could rewrite AC_GOTO
				action_table->action[i][item].num = graph.graph[i][item];
			}
		}
	}
	
	goto_table->size = graph.size;
	for (i = 0; i < graph.size; i++)
	{
		int j;
		for (j = 0; j < TOKENS; j++)
		{
			goto_table->goto_table[i][j] = graph.graph[i][j];
		}
	}
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
	
	action_table_t action_table;
	goto_table_t goto_table;
	build_automaton(&action_table, &goto_table);

	printf(".trans = (table_t*[])\n{\n");	
	for (i = 0; i < action_table.size; i++)
	{
		printf("\t(table_t[])\n\t{\n");
		int j;
		for (j = 0; j < TOKENS; j++)
		{
			printf("\t\t{");
			switch (action_table.action[i][j].action)
			{
				case AC_ERROR:
					printf("AC_ERROR, 0");
					break;
				case AC_ACCEPT:
					printf("AC_ACCEPT, 0");
					break;
				case AC_SHIFT:
					printf("AC_SHIFT, %d", action_table.action[i][j].num);
					break;
				case AC_REDUCE:
					printf("AC_REDUCE, %d", action_table.action[i][j].num);
					break;
				case AC_GOTO:
					printf("AC_GOTO, %d", action_table.action[i][j].num);
					break;
			}
			printf("},\n");
		}
		printf("\t},\n");	
	}

	printf("},\n.grammar_left = (item_t[])\n{\n\t");
	for (i = 0; i < grammar.count; i++)
	{
		printf("%d, ", grammar.rules[i].left);
	}
	
	printf("\n},\n.grammar_size = (int[])\n{\n\t");
	for (i = 0; i < grammar.count; i++)
	{
		printf("%d, ", grammar.rules[i].rule_size);
	}
	printf("\n}\n");
   




	
	/*
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
