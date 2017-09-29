#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <limits.h>

#include "values.h"
#include "scanner.lex.h"
#include "apply.h"
#include "executions.h"


apply_func_t apply_table[TERM_COUNT];
void apply(state_t result[], int* result_top, int num)
{
	apply_table[num](result, result_top);
}


bool solve(tables_t* table)
{
	init_apply();
	state_t input;
	input.token = yylex (&input.number);

	int state_top = 0;
	int state[100];
	state[0] = 0;

	state_t result[100];
	int result_top = -1;


	while (1)
	{
		int cur_state = state[state_top];
		table_t action = table->trans[cur_state][input.token];
		switch (action.action)
		{
			case AC_SHIFT:
				result[++result_top] = input;
				input.token = yylex (&input.number);
				state[++state_top] = action.num;
				break;
			case AC_REDUCE:
				apply(result, &result_top, action.num);
				state_top -= table->grammar_size[action.num];
				cur_state = state[state_top];
				state[++state_top] =
					table->trans[cur_state][table->grammar_left[action.num]].num;
				break;
			case AC_ERROR:
				printf("ERROR in %d state!\n", cur_state);
				return false;
			case AC_ACCEPT:
				printf("Answer: %d\n", result[0].number);
				return true;
		}
	}
}

int main(void)
{
	tables_t table =
	{
		#include "build_result.h"
	};
		
  
	if (!solve(&table))
		return (EXIT_FAILURE);

	return (EXIT_SUCCESS);
}
