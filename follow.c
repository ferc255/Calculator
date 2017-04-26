#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>

#define N 100

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
	NON_TERM_E,
	NON_TERM_E1,
	NON_TERM_T,
	NON_TERM_T1,
	NON_TERM_F,
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

grammar_t grammar = {
	.count = 8,
	.rules = (rule_t[]){
    {
		.left = NON_TERM_E,
		.rule_size = 3,
		.rule_elements = (rule_element_t[]) { {NON_TERM, NON_TERM_E}, {TERM, TERM_PLUS}, {NON_TERM, NON_TERM_T}, },
    }
  },
};

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

int find(char* token)
{
	int i;
	for (i = 0; i < list_size; i++)
	{
		if (strcmp(token_list[i], token) == 0)
		{
			return i;
		}
	}
}

void calcFirst(char* token)
{
	int idx = find(token);
	if (first_ptr[idx] != 0)
    {
		return;
    }

	bool found = false;
	int i;
	for (i = 0; i < n; i++)
    {
		if (strcmp(gramma_head[i], token) == 0)
		{
			int j;
			for (j = 0; j < gramma_size[i]; j++)
			{
				if (strcmp(gramma[i][j], token) == 0) // handle case then first (self) contains e
				{
					found = true;
					break;
				}
				calcFirst(gramma[i][j]); // handle case of infinite recursion 
				int neigh = find(gramma[i][j]);
				int z;
				for (z = 0; z < first_ptr[neigh]; z++)
				{
					if (!contains(first[idx], first_ptr[idx], first[neigh][z]))
					{
						strcpy(first[idx][first_ptr[idx]++], first[neigh][z]);
					}
				}

				if (!contains(first[neigh], first_ptr[idx], "e"))
				{
					found = true;
					break;
				}
			}

			if (!found && !contains(first[idx], first_ptr[idx], "e"))
			{
				strcpy(first[idx][first_ptr[idx]++], "e");
			}
		}
    }

	if (first_ptr[idx] == 0)
    {
		strcpy(first[idx][first_ptr[idx]++], token);
    }
}

void calcFollow(char* token)
{
	int idx = find(token);
	if (follow_ptr[idx] != 0)
    {
		return;
    }

	if (idx == 0)
    {
		strcpy(follow[idx][follow_ptr[idx]++], "$");
    }

	int i;
	for (i = 0; i < n; i++)
    {
		int j;
		for (j = 0; j < gramma_size[i]; j++)
		{
			if (strcmp(gramma[i][j], token) == 0)
			{
				int neigh;
				if (j + 1 < gramma_size[i]) //iterate over all non-terminals which contains e
				{
					neigh = find(gramma[i][j + 1]);
					int z;
					for (z = 0; z < first_ptr[neigh]; z++)
					{
						if (!contains(follow[idx], follow_ptr[idx], first[neigh][z]) &&
							strcmp(first[neigh][z], "e") != 0)
						{
							strcpy(follow[idx][follow_ptr[idx]++], first[neigh][z]);
						}
					}
				}

				if ((j + 1 == gramma_size[i] || contains(first[neigh], first_ptr[neigh], "e")) && // iterate over all following non-terminals (if all contains e in first)
					strcmp(gramma_head[i], token) != 0)
				{
					calcFollow(gramma_head[i]);
					int head = find(gramma_head[i]);
					int z;
					for (z = 0; z < follow_ptr[head]; z++)
					{
						if (!contains(follow[idx], follow_ptr[idx], follow[head][z]))
						{
							strcpy(follow[idx][follow_ptr[idx]++], follow[head][z]);
						}
					}
				}
			}
		}
    }
}


void add(char* token)
{
	int i;
	for (i = 0; i < list_size; i++)
	{
		if (strcmp(token_list[i], token) == 0)
		{
			return;
		}
	}

	//token_list[list_size++] = token;
	strcpy(token_list[list_size++], token);
}

int main()
{
	for (i = 0; i < list_size; i++)
	{
		calcFirst(i);
	}

	for (i = 0; i < list_size; i++)
	{
		calcFollow(token_list[i]);
	}

	for (i = 0; i < list_size; i++)
	{
		printf("%s\n", token_list[i]);
		int idx = find(token_list[i]);

		printf("FIRST: ");
		int j;
		for (j = 0; j < first_ptr[idx]; j++)
		{
			printf("%s ", first[idx][j]);
		}
		printf("\n");

		printf("FOLLOW: ");
		for (j = 0; j < follow_ptr[idx]; j++)
		{
			printf("%s ", follow[idx][j]);
		}
		printf("\n\n");
	}

	return 0;
}
