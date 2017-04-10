#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define N 100

int n;
int gramma_size[N];
char gramma_head[N][N];
char gramma[N][N][N];
char token_list[N][N];
int list_size = 0;
char first[N][N][N];
char follow[N][N][N];
int first_ptr[N];
int follow_ptr[N];

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
				if (strcmp(gramma[i][j], token) == 0)
				{
					found = true;
					break;
				}
				calcFirst(gramma[i][j]);
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
				if (j + 1 < gramma_size[i])
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

				if ((j + 1 == gramma_size[i] || contains(first[neigh], first_ptr[neigh], "e")) &&
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
	freopen("follow.txt", "r", stdin);

	scanf("%d", &n);

	int i;
	for (i = 0; i < n; i++)
	{
		char temp[N];
		scanf("%d %s", &gramma_size[i], gramma_head[i]);
		add(gramma_head[i]);

		int j;
		for (j = 0; j < gramma_size[i]; j++)
		{
			scanf("%s", gramma[i][j]);
			add(gramma[i][j]);
		}
	}

	for (i = 0; i < list_size; i++)
	{
		printf("%s\n", token_list[i]);
	}
	printf("\n");

	for (i = 0; i < list_size; i++)
	{
		calcFirst(token_list[i]);
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
