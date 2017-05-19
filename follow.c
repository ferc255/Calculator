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

void calcFirst(rule_element_t token, grammar_t* grammar, rec_state_t* used,
	first_t* first)
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
		first->first[token.item].list[0] = token.item;
		first->first[token.item].mask[token.item] = true;
		first->first[token.item].list_size = 1;
		used[token.item] = COUNTED;
		return;
	}

	used[token.item] = IN_PROGRESS;
	int i;
	for (i = 0; i < grammar->count; i++)
	{
		if (grammar->rules[i].left == token.item)
		{
			bool found = false;
			int j;
			
			for (j = 0; j < grammar->rules[i].rule_size; j++)
			{
				if (grammar->rules[i].rule_elements[j].item == token.item)
				{
					found = true;
					break;
				}
				
				calcFirst(grammar->rules[i].rule_elements[j], grammar, used,
					first);
				item_t neigh = grammar->rules[i].rule_elements[j].item;
				merge_sets (&first->first[token.item], &first->first[neigh]);

				if (!first->first[neigh].mask[TERM_EMPTY])
				{
					found = true;
					break;
				}
			}


			if (!found && !first->first[token.item].
				mask[TERM_EMPTY])
			{
				first->first[token.item].list[first->
					first[token.item].list_size++] = TERM_EMPTY;
				first->first[token.item].mask[TERM_EMPTY] = true;
			}				
		}
	}

	if (first->first[token.item].mask[TERM_EMPTY])
		for (i = 0; i < grammar->count; i++)
		{
			if (grammar->rules[i].left == token.item)
			{
				int j;
				for (j = 0; j < grammar->rules[i].rule_size; j++)
				{
					if (grammar->rules[i].rule_elements[j].item != token.item)
					{
						calcFirst(grammar->rules[i].rule_elements[j], grammar,
								  used, first);
					}
					item_t neigh = grammar->rules[i].rule_elements[j].item;
					merge_sets (&first->first[token.item], &first->first[neigh]);
	
					if (!first->first[neigh].mask[TERM_EMPTY])
					{
						break;
					}
				}
			}
		}

	used[token.item] = COUNTED;
}


void calcFollow(rule_element_t token, grammar_t* grammar, rec_state_t* used,
				first_t* first, follow_t* follow)
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
		follow->follow[token.item].list[0] = TERM_END;
		follow->follow[token.item].mask[TERM_END] = true;
		follow->follow[token.item].list_size = 1;
	}

	used[token.item] = IN_PROGRESS;
	int i;
	for (i = 0; i < grammar->count; i++)
	{
		bool contain_self = false;
		int j;
		for (j = 0; j < grammar->rules[i].rule_size; j++)
		{
			if (grammar->rules[i].rule_elements[j].item == token.item)
			{
				contain_self = true;
				break;
			}
		}
		
		bool has_nonempty = false;
		for (j = j + 1; j < grammar->rules[i].rule_size; j++)
		{
			item_t neigh = grammar->rules[i].rule_elements[j].item;
			merge_sets(&follow->follow[token.item], &first->first[neigh]);

			if (!first->first[neigh].mask[TERM_EMPTY])
			{
				has_nonempty = true;
				break;
			}
		}

		if (contain_self && !has_nonempty && grammar->rules[i].left != token.item)
		{
			rule_element_t left = {NON_TERM, grammar->rules[i].left};
			calcFollow(left, grammar, used, first, follow);
			item_t neigh = grammar->rules[i].left;
			merge_sets(&follow->follow[token.item], &follow->follow[neigh]);
		}
	}
	used[token.item] = COUNTED;
}


int comparator(const void* f, const void* s)
{
	point_t* a = ((point_t*) f);
	point_t* b = ((point_t*) s);
	if (a->grammar_num != b->grammar_num)
	{
		return a->grammar_num - b->grammar_num;
	}
	return a->position - b->position;
}

void add_points(int idx, grammar_t* grammar, scheme_t* scheme)
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
		if (point.position < grammar->rules[point.grammar_num].rule_size  &&
			grammar->rules[point.grammar_num].
			rule_elements[point.position].term_non_term	== NON_TERM)
		{
			item_t item = grammar->rules[point.grammar_num].
				rule_elements[point.position].item;
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
	scheme_t* scheme)
{
	bool found = false;
	scheme->scheme[graph->size].size = 0;
	int i;
	for (i = 0; i < scheme->scheme[idx].size; i++)
	{
		point_t point = scheme->scheme[idx].list[i];
		if (point.position < grammar->rules[point.grammar_num].rule_size &&
			grammar->rules[point.grammar_num].rule_elements[point.position].
			item == item)
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

	add_points(graph->size, grammar, scheme);
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


void build_scheme(grammar_t* grammar, graph_t* graph, scheme_t* scheme)
{
	graph->size = 1;
	scheme->scheme[0].list[0].grammar_num = 0,
	scheme->scheme[0].list[0].position = 0;
	scheme->scheme[0].size = 1;
	add_points(0, grammar, scheme);
	int i;
	for (i = 0; i < graph->size; i++) 
	{
		int j;
		for (j = 0; j < TOKENS; j++)
		{
	  		make_transition(i, j, grammar, graph, scheme);
		}
	}	
}

void build_automaton(action_table_t* action_table, goto_table_t* goto_table,
					 grammar_t* grammar, follow_t* follow, graph_t* graph,
					 scheme_t* scheme)
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
					rule_elements[point->position].item;
				action_table->action[i][item].action = AC_SHIFT; 
				action_table->action[i][item].num = graph->graph[i][item];
			}
		}
	}
	
	goto_table->size = graph->size;
	for (i = 0; i < graph->size; i++)
	{
		int j;
		for (j = 0; j < TOKENS; j++)
		{
			goto_table->goto_table[i][j] = graph->graph[i][j];
		}
	}
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
			calcFirst(grammar.rules[i].rule_elements[j], &grammar, used, &first);
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
			calcFollow(grammar.rules[i].rule_elements[j], &grammar, used,
					   &first, &follow);
		}
	}

	graph_t graph;
	scheme_t scheme;
	build_scheme(&grammar, &graph, &scheme);
	
	action_table_t action_table;
	goto_table_t goto_table;
	build_automaton(&action_table, &goto_table, &grammar, &follow, &graph,
					&scheme);

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
   




	return 0;
}
