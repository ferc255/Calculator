#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>

#include "values.h"


void init_set(set_t* set)
{
	int i;
	for (i = 0; i < TERM_COUNT; i++)
	{
		set->list_size = 0;

		int j;
		for (j = 0; j < TERM_COUNT; j++)
		{
			set->mask[j] = false;
		}
	}
}

void merge_sets(set_t * left, set_t * right)
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

void calc_first(item_t token, grammar_t* grammar, rec_state_t* used,
				first_t* first, term_non_term_t* term_non_term)
{
	if (used[token] == COUNTED)
	{
		return;
	}
	if (used[token] == IN_PROGRESS)
	{
		printf("Invalid grammar\n");
		exit(0);
	}
	if (term_non_term[token] == TERM)
	{
		first->first[token].list[0] = token;
		first->first[token].mask[token] = true;
		first->first[token].list_size = 1;
		used[token] = COUNTED;
		return;
	}

	used[token] = IN_PROGRESS;
	int i;
	for (i = 0; i < grammar->count; i++)
	{
		if (grammar->rules[i].left == token)
		{
			bool found = false;
			int j;
			
			for (j = 0; j < grammar->rules[i].rule_size; j++)
			{
				if (grammar->rules[i].rule_elements[j] == token)
				{
					found = true;
					break;
				}
				
				calc_first(grammar->rules[i].rule_elements[j], grammar, used,
						   first, term_non_term);
				item_t neigh = grammar->rules[i].rule_elements[j];
				merge_sets (&first->first[token], &first->first[neigh]);

				if (!first->first[neigh].mask[TERM_EMPTY])
				{
					found = true;
					break;
				}
			}


			if (!found && !first->first[token].
				mask[TERM_EMPTY])
			{
				first->first[token].list[first->
					first[token].list_size++] = TERM_EMPTY;
				first->first[token].mask[TERM_EMPTY] = true;
			}				
		}
	}

	if (first->first[token].mask[TERM_EMPTY])
		for (i = 0; i < grammar->count; i++)
		{
			if (grammar->rules[i].left == token)
			{
				int j;
				for (j = 0; j < grammar->rules[i].rule_size; j++)
				{
					if (grammar->rules[i].rule_elements[j] != token)
					{
						calc_first(grammar->rules[i].rule_elements[j], grammar,
								   used, first, term_non_term);
					}
					item_t neigh = grammar->rules[i].rule_elements[j];
					merge_sets (&first->first[token], &first->first[neigh]);
	
					if (!first->first[neigh].mask[TERM_EMPTY])
					{
						break;
					}
				}
			}
		}

	used[token] = COUNTED;
}


void calc_follow(item_t token, grammar_t* grammar, rec_state_t* used,
				 first_t* first, follow_t* follow)
{
	if (used[token] == COUNTED)
	{
		return;
	}
	if (used[token] == IN_PROGRESS)
	{
		printf("Invalid grammar\n");
		exit(0);
	}
	if (token == NON_TERM_S)
	{
		follow->follow[token].list[0] = TERM_END;
		follow->follow[token].mask[TERM_END] = true;
		follow->follow[token].list_size = 1;
	}

	used[token] = IN_PROGRESS;
	int i;
	for (i = 0; i < grammar->count; i++)
	{
		bool contain_self = false;
		int j;
		for (j = 0; j < grammar->rules[i].rule_size; j++)
		{
			if (grammar->rules[i].rule_elements[j] == token)
			{
				contain_self = true;
				break;
			}
		}
		
		bool has_nonempty = false;
		for (j = j + 1; j < grammar->rules[i].rule_size; j++)
		{
			item_t neigh = grammar->rules[i].rule_elements[j];
			merge_sets(&follow->follow[token], &first->first[neigh]);

			if (!first->first[neigh].mask[TERM_EMPTY])
			{
				has_nonempty = true;
				break;
			}
		}

		if (contain_self && !has_nonempty && grammar->rules[i].left != token)
		{
			item_t left = grammar->rules[i].left;
			calc_follow(left, grammar, used, first, follow);
			merge_sets(&follow->follow[token], &follow->follow[left]);
		}
	}
	used[token] = COUNTED;
}


int comparator(const void* f, const void* s)
{
	point_t* a = ((point_t*) f);
	point_t* b = ((point_t*) s);
	int div = a->grammar_num - b->grammar_num;
	if (div)
	{
		return div;
	}
	return a->position - b->position;
}

void add_points(int idx, grammar_t* grammar, scheme_t* scheme,
				term_non_term_t* term_non_term)
{
	bool added[TERM_COUNT];
	int i;
	for (i = 0; i < TERM_COUNT; i++)
	{
		added[i] = false;
	}

	for (i = 0; i < scheme->scheme[idx].size; i++)
	{
		point_t point = scheme->scheme[idx].list[i];
		if (point.position < grammar->rules[point.grammar_num].rule_size &&
			term_non_term[grammar->rules[point.grammar_num].
			rule_elements[point.position]] == NON_TERM)
		{
			item_t item = grammar->rules[point.grammar_num].
				rule_elements[point.position];
			if (!added[item])
			{
				added[item] = true;
				int j;
				for (j = 0; j < grammar->count; j++)
				{
					if (grammar->rules[j].left == item)
					{
						scheme->scheme[idx].list[scheme->scheme[idx].size]
							.grammar_num = j;
						scheme->scheme[idx].list[scheme->scheme[idx].size]
							.position = 0;
					
						scheme->scheme[idx].size++;
					}
				}
			}
		}
	}

	qsort(scheme->scheme[idx].list, scheme->scheme[idx].size,
		  sizeof(point_t), comparator);
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
		if (a.list[i].grammar_num != b.list[i].grammar_num ||
			a.list[i].position != b.list[i].position)
		{
			return false;
		}
	}

	return true;
}

void make_transition(int idx, item_t item, grammar_t* grammar, graph_t* graph,
					 scheme_t* scheme, term_non_term_t* term_non_term)
{
	bool found = false;
	scheme->scheme[graph->size].size = 0;
	int i;
	for (i = 0; i < scheme->scheme[idx].size; i++)
	{
		point_t point = scheme->scheme[idx].list[i];
		if (point.position < grammar->rules[point.grammar_num].rule_size &&
			grammar->rules[point.grammar_num].
			rule_elements[point.position] == item)
		{
			found = true;
			scheme->scheme[graph->size].list[scheme->scheme[graph->size].size]
				.grammar_num = point.grammar_num;
			scheme->scheme[graph->size].list[scheme->scheme[graph->size].size]
				.position = point.position + 1;
			scheme->scheme[graph->size].size++;
		}
	}

	if (!found)
	{
		return;
	}

	add_points(graph->size, grammar, scheme, term_non_term);
	found = false;
	for (i = 0; i < graph->size; i++)
	{
		if (is_equal_states(scheme->scheme[i], scheme->scheme[graph->size]))
		{
			found = true;
			graph->graph[idx][item] = i;
			break;
		}
	}

	if (!found)
	{
		graph->graph[idx][item] = graph->size;
		graph->size++;
	}
}


void build_scheme(grammar_t* grammar, graph_t* graph, scheme_t* scheme,
				  term_non_term_t* term_non_term)
{
	graph->size = 1;
	scheme->scheme[0].list[0].grammar_num = 0,
	scheme->scheme[0].list[0].position = 0;
	scheme->scheme[0].size = 1;
	add_points(0, grammar, scheme, term_non_term);
	int i;
	for (i = 0; i < graph->size; i++) 
	{
		int j;
		for (j = 0; j < TOKENS; j++)
		{
	  		make_transition(i, j, grammar, graph, scheme, term_non_term);
		}
	}	
}

void build_automaton(action_table_t* action_table, grammar_t* grammar,
					 follow_t* follow, graph_t* graph, scheme_t* scheme)
{
	action_table->size = graph->size;
	
	int i;
	for (i = 0; i < graph->size; i++)
	{
		int j;
		for (j = 0; j < TOKENS; j++)
		{
			if (graph->graph[i][j] == 0)
			{
				action_table->action[i][j].action = AC_ERROR;
			}
			else
			{
				action_table->action[i][j].action = AC_GOTO;
				action_table->action[i][j].num = graph->graph[i][j];
			}
		}
	}
	
	
	
	for (i = 0; i < graph->size; i++)
	{
		int j;
		for (j = 0; j < scheme->scheme[i].size; j++)
		{
			point_t* point = &scheme->scheme[i].list[j];
			rule_t* rule = &grammar->rules[point->grammar_num];
			if (point->position == rule->rule_size)
			{
				if (point->grammar_num == 0)
				{
					action_table->action[i][TERM_END].action = AC_ACCEPT;
				}
				else
				{
					int k;
					for (k = 0; k < follow->follow[rule->left].list_size; k++)
					{
						item_t item = follow->follow[rule->left].list[k];
						action_table->action[i][item].action = AC_REDUCE;
						action_table->action[i][item].num = point->grammar_num;
					}
				}
			}
			else
			{
				item_t item = grammar->rules[point->grammar_num].
					rule_elements[point->position];
				action_table->action[i][item].action = AC_SHIFT; 
				action_table->action[i][item].num = graph->graph[i][item];
			}
		}
	}
}


void write_tables(action_table_t* action_table, grammar_t* grammar)
{
	printf(".trans = (table_t*[])\n{\n");
	int i;
	for (i = 0; i < action_table->size; i++)
	{
		printf("\t(table_t[])\n\t{\n");
		int j;
		for (j = 0; j < TOKENS; j++)
		{
			printf("\t\t{");
			switch (action_table->action[i][j].action)
			{
				case AC_ERROR:
					printf("AC_ERROR, 0");
					break;
				case AC_ACCEPT:
					printf("AC_ACCEPT, 0");
					break;
				case AC_SHIFT:
					printf("AC_SHIFT, %d", action_table->action[i][j].num);
					break;
				case AC_REDUCE:
					printf("AC_REDUCE, %d", action_table->action[i][j].num);
					break;
				case AC_GOTO:
					printf("AC_GOTO, %d", action_table->action[i][j].num);
					break;
			}
			printf("},\n");
		}
		printf("\t},\n");	
	}

	printf("},\n.grammar_left = (item_t[])\n{\n\t");
	for (i = 0; i < grammar->count; i++)
	{
		printf("%d, ", grammar->rules[i].left);
	}
	
	printf("\n},\n.grammar_size = (int[])\n{\n\t");
	for (i = 0; i < grammar->count; i++)
	{
		printf("%d, ", grammar->rules[i].rule_size);
	}
	printf("\n}\n");
}


int main()
{
	grammar_t grammar =
	{
		.count = 7,
		.rules = (rule_t[])
		{
			{
				.left = NON_TERM_S,
				.rule_size = 1,
				.rule_elements = (item_t[])
				{
					NON_TERM_E,
				},
			},
			{
				.left = NON_TERM_E,
				.rule_size = 3,
				.rule_elements = (item_t[])
				{
					NON_TERM_E,	TERM_PLUS, NON_TERM_T,
				},
			},
			{
				.left = NON_TERM_E,
				.rule_size = 1,
				.rule_elements = (item_t[])
				{
					NON_TERM_T,
				},
			},
			{
				.left = NON_TERM_T,
				.rule_size = 3,
				.rule_elements = (item_t[])
				{
					NON_TERM_T, TERM_MUL, NON_TERM_F,
				},
			},
			{
				.left = NON_TERM_T,
				.rule_size = 1,
				.rule_elements = (item_t[])
				{
					NON_TERM_F,
				},
			},
			{
				.left = NON_TERM_F,
				.rule_size = 3,
				.rule_elements = (item_t[])
				{
					TERM_LPAREN, NON_TERM_E, TERM_RPAREN,
				},
			},
			{
				.left = NON_TERM_F,
				.rule_size = 1,
				.rule_elements = (item_t[])
				{
					TERM_ID,
				},
			},
		}
	};
	term_non_term_t* term_non_term = (term_non_term_t[])
	{
		TERM,
		TERM,
		TERM,
		TERM,
		TERM,
		TERM,
		TERM,
		NON_TERM,
		NON_TERM,
		NON_TERM,
		NON_TERM,
	};

	int i;
	first_t first;
	follow_t follow;
	for (i = 0; i < TERM_COUNT; i++)
	{
		init_set(&first.first[i]);
		init_set(&follow.follow[i]);
	}

	rec_state_t used[TERM_COUNT];
	for (i = 0; i < TERM_COUNT; i++)
	{
		used[i] = NOT_COUNTED;
	}
	for (i = 0; i < grammar.count; i++)
	{
		int j;
		for (j = 0; j < grammar.rules[i].rule_size; j++)
		{
			calc_first(grammar.rules[i].rule_elements[j], &grammar, used,
					   &first, term_non_term);
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
			calc_follow(grammar.rules[i].rule_elements[j], &grammar, used,
					   &first, &follow);
		}
	}

	graph_t graph;
	scheme_t scheme;
	build_scheme(&grammar, &graph, &scheme, term_non_term);
	
	action_table_t action_table;
	build_automaton(&action_table, &grammar, &follow, &graph,
					&scheme);

	write_tables(&action_table, &grammar);





	return 0;
}
