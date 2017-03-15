#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <limits.h>



typedef enum {
  SHIFT,
  REDUCE,
  ACC,
  ERR
} action_t;

typedef struct {
  action_t action;
  int num;
} table_t;

typedef enum
{
	NUMBER,
	PLUS,
	MUL,
	LPAREN,
	RPAREN,
	END,
	MINUS,
	DIV,
} token_t;

typedef struct
{
	token_t token;
	int number;
} state_t;

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

static void
lex (state_t * input)
{
	int c2t[256] = 
	{
		[0 ... 255] = END,
		['+'] = PLUS,
		['*'] = MUL,
		['('] = LPAREN,
		[')'] = RPAREN,
		['-'] = MINUS,
		['/'] = DIV,
	};
  int c;


  input->token = END;
  for (;;)
    {
      c = getchar ();
      if (EOF == c)
	break;
      if (isspace (c))
	continue;
      if (isdigit (c))
	{
	  ungetc (c, stdin);
	  fscanf (stdin, "%d", &input->number);
	  input->token = NUMBER;
	}
      else
	{
	  input->token = c2t[c];
	}
      break;
    }
}

bool solve(table_t table[16][8], int trans[16][3])
{
  state_t input;
  lex (&input);

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
	case SHIFT:
	  result[++result_top] = input;
	  lex (&input);
	  state[++state_top] = action.num;
	  break;
	case REDUCE:
	  apply(result, &result_top, action.num);
	  state_top -= gramma_size[action.num];
	  cur_state = state[state_top];
	  state[++state_top] = trans[cur_state][gramma_head[action.num]];
	  break;
	case ERR:
	  printf("ERROR in %d state!\n", cur_state);
	  return false;
	case ACC:
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
	[NUMBER] = {SHIFT, 5},
	[PLUS] = {ERR, 0},
	[MUL] = {ERR, 0},
	[LPAREN] = {SHIFT, 4},
	[RPAREN] = {ERR, 0},
	[END] = {ERR, 0},
	[MINUS] = {ERR, 0},
	[DIV] = {ERR, 0},
      },
      {{ERR, 0}, {SHIFT, 6}, {ERR, 0}, {ERR, 0}, {ERR, 0}, {ACC, 0}, {SHIFT, 15}, {ERR, 0}},
      {{ERR, 0}, {REDUCE, 2}, {SHIFT, 7}, {ERR, 0}, {REDUCE, 2}, {REDUCE, 2}, {REDUCE, 2}, {SHIFT, 14}},
      {{ERR, 0}, {REDUCE, 4}, {REDUCE, 4}, {ERR, 0}, {REDUCE, 4}, {REDUCE, 4}, {REDUCE, 4}, {REDUCE, 4}},
      {{SHIFT, 5}, {ERR, 0}, {ERR, 0}, {SHIFT, 4}, {ERR, 0}, {ERR, 0}, {ERR, 0}, {ERR, 0}},
      {{ERR, 0}, {REDUCE, 6}, {REDUCE, 6}, {ERR, 0}, {REDUCE, 6}, {REDUCE, 6}, {REDUCE, 6}, {REDUCE, 6}},
      {{SHIFT, 5}, {ERR, 0}, {ERR, 0}, {SHIFT, 4}, {ERR, 0}, {ERR, 0}, {ERR, 0}, {ERR, 0}},
      {{SHIFT, 5}, {ERR, 0}, {ERR, 0}, {SHIFT, 4}, {ERR, 0}, {ERR, 0}, {ERR, 0}, {ERR, 0}},
      {{ERR, 0}, {SHIFT, 6}, {ERR, 0}, {ERR, 0}, {SHIFT, 11}, {ERR, 0}, {SHIFT, 15}, {ERR, 0}},
      {{ERR, 0}, {REDUCE, 1}, {SHIFT, 7}, {ERR, 0}, {REDUCE, 1}, {REDUCE, 1}, {REDUCE, 1}, {SHIFT, 14}},
      {{ERR, 0}, {REDUCE, 3}, {REDUCE, 3}, {ERR, 0}, {REDUCE, 3}, {REDUCE, 3}, {REDUCE, 3}, {REDUCE, 3}},
      {{ERR, 0}, {REDUCE, 5}, {REDUCE, 5}, {ERR, 0}, {REDUCE, 5}, {REDUCE, 5}, {REDUCE, 5}, {REDUCE, 5}},
      {{ERR, 0}, {REDUCE, 9}, {REDUCE, 9}, {ERR, 0}, {REDUCE, 9}, {REDUCE, 9}, {REDUCE, 9}, {REDUCE, 9}},
      {{ERR, 0}, {REDUCE, 7}, {SHIFT, 7}, {ERR, 0}, {REDUCE, 7}, {REDUCE, 7}, {REDUCE, 7}, {SHIFT, 14}},
      {{SHIFT, 5}, {ERR, 0}, {ERR, 0}, {SHIFT, 4}, {ERR, 0}, {ERR, 0}, {ERR, 0}, {ERR, 0}},
      {{SHIFT, 5}, {ERR, 0}, {ERR, 0}, {SHIFT, 4}, {ERR, 0}, {ERR, 0}, {ERR, 0}, {ERR, 0}}
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
