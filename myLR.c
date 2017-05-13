#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <limits.h>

#include "values.h"
#include "scanner.lex.h"

typedef struct {
  action_t action;
  int num;
} table_t;

void apply(state_t result[], int* result_top, int num)
{
  if (num % 2 == 0)
    {
      return;
    }

  switch (num)
    {
    case 1:
      result[*result_top - 2].number += result[*result_top].number;
      break;
    case 3:
      result[*result_top - 2].number *= result[*result_top].number;
      break;
    case 5:
      result[*result_top - 2].number = result[*result_top - 1].number;
      break;
    case 7:
      result[*result_top - 2].number -= result[*result_top].number;
      break;
    case 9:
      result[*result_top - 2].number /= result[*result_top].number;
      break;
    }

  *result_top -= 2;
}


bool solve(table_t table[16][8], int trans[16][3])
{
  state_t input;
  input.token = yylex (&input.number);

  int state_top = 0;
  int state[100];
  state[0] = 0;

  state_t result[100];
  int result_top = -1;

  int gramma_size[10] = { 0, 3, 1, 3, 1, 3, 1, 3, 0, 3 };
  int gramma_head[10] = { -1, 0, 0, 1, 1, 2, 2, 0, -1, 1 };

  while (1)
    {
      int cur_state = state[state_top];
      table_t action = table[cur_state][input.token];
      switch (action.action)
	{
	case AC_SHIFT:
	  result[++result_top] = input;
	  input.token = yylex (&input.number);
	  state[++state_top] = action.num;
	  break;
	case AC_REDUCE:
	  apply(result, &result_top, action.num);
	  state_top -= gramma_size[action.num];
	  cur_state = state[state_top];
	  state[++state_top] = trans[cur_state][gramma_head[action.num]];
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
  table_t table[16][8] =
    {
      {
	[NUMBER] = {AC_SHIFT, 5},
	[PLUS] = {AC_ERROR, 0},
	[MUL] = {AC_ERROR, 0},
	[LPAREN] = {AC_SHIFT, 4},
	[RPAREN] = {AC_ERROR, 0},
	[END] = {AC_ERROR, 0},
	[MINUS] = {AC_ERROR, 0},
	[DIV] = {AC_ERROR, 0},
      },
      {{AC_ERROR, 0}, {AC_SHIFT, 6}, {AC_ERROR, 0}, {AC_ERROR, 0}, {AC_ERROR, 0}, {AC_ACCEPT, 0}, {AC_SHIFT, 15}, {AC_ERROR, 0}},
      {{AC_ERROR, 0}, {AC_REDUCE, 2}, {AC_SHIFT, 7}, {AC_ERROR, 0}, {AC_REDUCE, 2}, {AC_REDUCE, 2}, {AC_REDUCE, 2}, {AC_SHIFT, 14}},
      {{AC_ERROR, 0}, {AC_REDUCE, 4}, {AC_REDUCE, 4}, {AC_ERROR, 0}, {AC_REDUCE, 4}, {AC_REDUCE, 4}, {AC_REDUCE, 4}, {AC_REDUCE, 4}},
      {{AC_SHIFT, 5}, {AC_ERROR, 0}, {AC_ERROR, 0}, {AC_SHIFT, 4}, {AC_ERROR, 0}, {AC_ERROR, 0}, {AC_ERROR, 0}, {AC_ERROR, 0}},
      {{AC_ERROR, 0}, {AC_REDUCE, 6}, {AC_REDUCE, 6}, {AC_ERROR, 0}, {AC_REDUCE, 6}, {AC_REDUCE, 6}, {AC_REDUCE, 6}, {AC_REDUCE, 6}},
      {{AC_SHIFT, 5}, {AC_ERROR, 0}, {AC_ERROR, 0}, {AC_SHIFT, 4}, {AC_ERROR, 0}, {AC_ERROR, 0}, {AC_ERROR, 0}, {AC_ERROR, 0}},
      {{AC_SHIFT, 5}, {AC_ERROR, 0}, {AC_ERROR, 0}, {AC_SHIFT, 4}, {AC_ERROR, 0}, {AC_ERROR, 0}, {AC_ERROR, 0}, {AC_ERROR, 0}},
      {{AC_ERROR, 0}, {AC_SHIFT, 6}, {AC_ERROR, 0}, {AC_ERROR, 0}, {AC_SHIFT, 11}, {AC_ERROR, 0}, {AC_SHIFT, 15}, {AC_ERROR, 0}},
      {{AC_ERROR, 0}, {AC_REDUCE, 1}, {AC_SHIFT, 7}, {AC_ERROR, 0}, {AC_REDUCE, 1}, {AC_REDUCE, 1}, {AC_REDUCE, 1}, {AC_SHIFT, 14}},
      {{AC_ERROR, 0}, {AC_REDUCE, 3}, {AC_REDUCE, 3}, {AC_ERROR, 0}, {AC_REDUCE, 3}, {AC_REDUCE, 3}, {AC_REDUCE, 3}, {AC_REDUCE, 3}},
      {{AC_ERROR, 0}, {AC_REDUCE, 5}, {AC_REDUCE, 5}, {AC_ERROR, 0}, {AC_REDUCE, 5}, {AC_REDUCE, 5}, {AC_REDUCE, 5}, {AC_REDUCE, 5}},
      {{AC_ERROR, 0}, {AC_REDUCE, 9}, {AC_REDUCE, 9}, {AC_ERROR, 0}, {AC_REDUCE, 9}, {AC_REDUCE, 9}, {AC_REDUCE, 9}, {AC_REDUCE, 9}},
      {{AC_ERROR, 0}, {AC_REDUCE, 7}, {AC_SHIFT, 7}, {AC_ERROR, 0}, {AC_REDUCE, 7}, {AC_REDUCE, 7}, {AC_REDUCE, 7}, {AC_SHIFT, 14}},
      {{AC_SHIFT, 5}, {AC_ERROR, 0}, {AC_ERROR, 0}, {AC_SHIFT, 4}, {AC_ERROR, 0}, {AC_ERROR, 0}, {AC_ERROR, 0}, {AC_ERROR, 0}},
      {{AC_SHIFT, 5}, {AC_ERROR, 0}, {AC_ERROR, 0}, {AC_SHIFT, 4}, {AC_ERROR, 0}, {AC_ERROR, 0}, {AC_ERROR, 0}, {AC_ERROR, 0}}
    };

  int trans[16][3] = {
    [0 ... 15] = { [0 ... 2] = 0, },
    [0] = {1, 2, 3},
    [4] = {8, 2, 3},
    [6] = {0, 9, 3},
    [7] = {0, 0, 10},
    [14] = {0, 0, 12},
    [15] = {0, 13, 3},
  };
  
  if (!solve(table, trans))
    return (EXIT_FAILURE);

  return (EXIT_SUCCESS);
}
