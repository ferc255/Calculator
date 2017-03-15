#include <stdio.h>
#include <string.h>
#include <stdbool.h>

const int N = 100;

typedef enum
{
	SHIFT,
	REDUCE,
	ACC,
	ERR
} action_t;

typedef struct
{
	action_t type;
	int num;
} table_t;

typedef enum
{
	DIGIT,
	CHAR
} data_t;

typedef enum
{
	INT,
	PLUS,
	MULTIP,
	OPEN,
	CLOSE,
	END,
	MINUS,
	DIVIS
} sign_t;

typedef struct
{
	data_t type;
	sign_t col;
	int integer;
} token_t;

void apply(token_t result[], int* result_top, int num)
{
	if (num % 2 == 0)
	{
		return;
	}

	switch (num)
	{
	case 1:
		result[*result_top - 2].integer += result[*result_top].integer;
		break;
	case 3:
		result[*result_top - 2].integer *= result[*result_top].integer;
		break;
	case 5:
		result[*result_top - 2].integer = result[*result_top - 1].integer;
		break;
	case 7:
		result[*result_top - 2].integer -= result[*result_top].integer;
		break;
	case 9:
		result[*result_top - 2].integer /= result[*result_top].integer;
		break;
	}

	*result_top -= 2;
}

int to_digit(char temp[], int end)
{
	char s[N];
	int i;
	for (i = 1; i < end; i++)
	{
		s[i - 1] = temp[i];
	}

	return atoi(s);
}

bool read_input_line(token_t input[])
{
	int n;
	scanf("%d", &n);
	int i;
	for (i = 0; i < n; i++)
	{
		char temp[N];
		scanf("%s", temp);
		if (isdigit(temp[0]))
		{
			input[i].type = DIGIT;
			input[i].col = INT;
			input[i].integer = atoi(temp);
		}
		else
		{
			input[i].type = CHAR;
			switch (temp[0])
			{
			case '+':
				input[i].col = PLUS;
				break;
			case '*':
				input[i].col = MULTIP;
				break;
			case '(':
				input[i].col = OPEN;
				break;
			case ')':
				input[i].col = CLOSE;
				break;
			case '-':
				input[i].col = MINUS;
				break;
			case '/':
				input[i].col = DIVIS;
				break;
			default:
				printf("Error during parcing input at %dth token\n", i);
				return false;
			}
		}
	}
	input[n].col = END;
	return true;
}

bool fill_table(table_t table[16][8], int trans[16][3])
{
	int i;
	for (i = 0; i < 16; i++)
	{
		int j;
		for (j = 0; j < 8; j++)
		{
			char temp[5];
			scanf("%s", temp);
			if (temp[0] == '0')
			{
				table[i][j].type = ERR;
			}
			else
			{
				int z = 1;
				while (isdigit(temp[z]))
				{
					z++;
				}
				table[i][j].num = to_digit(temp, z);

				if (temp[0] == 's')
				{
					table[i][j].type = SHIFT;
				}
				else if (temp[0] == 'r')
				{
					table[i][j].type = REDUCE;
				}
				else
				{
					printf("Error during parsing table at %d %d position\n", i, j);
					return false;
				}
			}
		}
	}
	table[1][5].type = ACC;

	trans[0][0] = 1;
	trans[0][1] = trans[4][1] = 2;
	trans[0][2] = trans[4][2] = trans[6][2] = 3;
	trans[4][0] = 8;
	trans[6][1] = 9;
	trans[7][2] = 10;
	trans[14][2] = 12;
	trans[15][1] = 13;
	trans[15][2] = 3;
	return true;
}

bool solve(token_t input[], table_t table[16][8], int trans[16][3])
{
	int input_idx = 0;

	int state_top = 0;
	int state[100];
	state[0] = 0;

	token_t result[100];
	int result_top = -1;

	int gramma_size[10] = { 0, 3, 1, 3, 1, 3, 1, 3, 0, 3 };
	int gramma_head[10] = { -1, 0, 0, 1, 1, 2, 2, 0, -1, 1 };

	while (1)
	{
		int cur_state = state[state_top];
		table_t action = table[cur_state][input[input_idx].col];
		switch (action.type)
		{
		case SHIFT:
			result[++result_top] = input[input_idx++];
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
			printf("Answer: %d\n", result[0].integer);
			return true;
		}
	}
}

int main(void)
{
	freopen("input.txt", "r", stdin);

	token_t input[100];	
	if (!read_input_line(input)) return 0;	

	table_t table[16][8];
	int trans[16][3];
	if (!fill_table(table, trans)) return 0;	

	if (!solve(input, table, trans)) return 0;

	return 0;
}
