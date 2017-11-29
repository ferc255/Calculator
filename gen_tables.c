#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "values.h"

bool contains(first_t* first, int elem)
{
    int i;
    for (i = 0; i < first->size; i++)
    {
        if (first->list[i] == elem)
        {
            return true;
        }
    }
    
    return false;
}

void merge_first_sets(first_t* left, first_t* right)
{
    int i;
    for (i = 0; i < right->size; i++)
    {
        if (!contains(left, right->list[i]))
        {
            left->list[left->size++] = right->list[i];
        }
    }
}

void calc_first(token_id_t token, grammar_t* grammar, first_t* first)
{
    if (first[token].size)
    {
        return;
    }
    
    bool is_terminal = true;
    int i;
    for (i = 0; i < grammar->size; i++)
    {
        if (grammar->prod[i].left == token && 
            grammar->prod[i].list[0] != token)
        {
            is_terminal = false;
            int neigh = grammar->prod[i].list[0];
            calc_first(neigh, grammar, first);
            merge_first_sets(&first[token], &first[neigh]);
        }
    }
    
    if (is_terminal)
    {
        first[token].list[first[token].size++] = token;
    }
}

void malloc_added(bool*** added, int tokens_count)
{
    *added = malloc(tokens_count * sizeof(bool*));
    int i;
    for (i = 0; i < tokens_count; i++)
    {
        (*added)[i] = malloc(tokens_count * sizeof(bool));
        int j;
        for (j = 0; j < tokens_count; j++)
        {
            (*added)[i][j] = false;
        }
    }
}

void add_items(token_id_t head, token_id_t end, item_list_t* set, bool** added,
                grammar_t* grammar)
{
    if (added[head][end]) return;
    
    int i;
    for (i = 0; i < grammar->size; i++)
    {
        if (grammar->prod[i].left == head)
        {
            set->list[set->size].prod = i;
            set->list[set->size].pos = 0;
            set->list[set->size].end = end;
            set->size++;
        }
    }
    
    added[head][end] = true;
}

void closure(int idx, grammar_t* grammar, item_list_t* scheme, first_t* first)
{
    bool** added;
    malloc_added(&added, grammar->token_names.size);
    
    int i;
    for (i = 0; i < scheme[idx].size; i++)
    {
        item_t item = scheme[idx].list[i];
        if (item.pos == grammar->prod[item.prod].size) continue;
        token_id_t head = grammar->prod[item.prod].list[item.pos];
        if (item.pos + 1 < grammar->prod[item.prod].size)
        {
            token_id_t neigh = grammar->prod[item.prod].list[item.pos + 1];
            calc_first(neigh, grammar, first);
            int j;
            for (j = 0; j < first[neigh].size; j++)
            {
                add_items(head, first[neigh].list[j], &scheme[idx], added, 
                    grammar);
            }
        }
        else
        {
            add_items(head, item.end, &scheme[idx], added, grammar);
        }
    }
}

void build_automaton(grammar_t* grammar, item_list_t* scheme, transit_t* transit,
    first_t* first)
{
    
    scheme[0].list[0].prod =
        scheme[0].list[0].pos =
            scheme[0].list[0].end = 0;
    scheme[0].size = 1;
    transit->size = 1;
    closure(0, grammar, scheme, first);
}


void malloc_first(first_t** first, int tokens_count)
{
    *first = malloc(tokens_count * sizeof(first_t*));
    int i;
    for (i = 0; i < tokens_count; i++)
    {
        (*first)[i].list = malloc(tokens_count * sizeof(int));
    }
}


int main()
{
    grammar_t grammar = 
    {
        .token_names = 
        {
            .size = 6,
            .list = (char*[])
            {
                "$",
                "^",
                "E",
                "PLUS",
                "NUM",
                "MINUS",
            },
        },
        .prod = (production_t[])
        {
            {
                .left = 1,
                .list = (int[])
                {
                    2,
                },
                .size = 1,
            },
            {
                .left = 2,
                .list = (int[])
                {
                    2, 3, 4
                },
                .size = 3,
            },
            {
                .left = 2,
                .list = (int[])
                {
                    2, 5, 4
                },
                .size = 3,
            },
            {
                .left = 2,
                .list = (int[])
                {
                    4,
                },
                .size = 1,
            },
        },
        .size = 4,
    };
    
    first_t* first;
    malloc_first(&first, grammar.token_names.size);
    
    int i;
    for (i = 0; i < grammar.token_names.size; i++)
    {
        calc_first(i, &grammar, first);
        printf("%s: ", grammar.token_names.list[i]);
        int j;
        for (j = 0; j < first[i].size; j++)
        {
            printf("%s ", grammar.token_names.list[first[i].list[j]]);
        }
        printf("\n");
    }
    
    transit_t transit;
    item_list_t scheme[MAX_STATES];
    build_automaton(&grammar, scheme, &transit, first);
    
    
    for (i = 0; i < scheme[0].size; i++)
    {
        printf("%d %d %d\n", scheme[0].list[i].prod, scheme[0].list[i].pos, scheme[0].list[i].end);
    }
    
}