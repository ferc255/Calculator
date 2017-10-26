#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N 200

const int ERROR_STATE = -1;
const int NOT_FINAL = -1;

typedef enum
{
    NT_CHAR,
    NT_CAT,
    NT_STAR,
    NT_OR,
    NT_END,
    NT_LPAREN,
    NT_RPAREN,
    NT_EPS,
} node_type_t;

typedef struct rule_token_t
{
    node_type_t type;
    char symbol;
} rule_token_t;

typedef struct lex_rule_t
{
    char* abbrev;
    int prior;
    int size;
    rule_token_t* list;
} lex_rule_t;

typedef struct buffer_t
{
    int count;
    lex_rule_t* rule;
} buffer_t;

typedef struct node_t
{
    int index;
    node_type_t type;
    char symbol;
    int prior;
    bool is_nullable;
    int first_ptr, last_ptr, follow_ptr;
    struct node_t* first[N];
    struct node_t* last[N];
    struct node_t* follow[N];
    struct node_t* left;
    struct node_t* right;
    struct node_t* parent;
} node_t;


int default_prior[] = 
{
    [NT_CHAR] = 0,
    [NT_END] = 0,
    [NT_EPS] = 0,
    [NT_STAR] = 1,
    [NT_CAT] = 2,
    [NT_OR] = 3,
    [NT_LPAREN] = 4,
    [NT_RPAREN] = 4,
};




void print_arr(node_t* arr[1000], int *sz)
{
    int i;
    for (i = 0; i < *sz; i++)
    {
        printf("%p ", arr[i]);
    }
    printf("\n");
}


void print_dfs(node_t* v)
{
    if (v->type)
    {
        printf("[%d]\n", v->type);
    }
    else
    {
        printf("[\'%c\']\n", v->symbol);
    }
    if (v->left)
    {
        printf("LF\n");
        print_dfs(v->left);
    }
    if (v->right)
    {
        printf("RG\n");
        print_dfs(v->right);
    }
    if (v->type)
    {
        printf("[%d] ", v->type);
    }
    else
    {
        printf("[\'%c\'] ", v->symbol);
    }
    printf("EXIT\n");

    //print_arr(v->first, &v->first_ptr);
    print_arr(v->follow, &v->follow_ptr);
    printf("====\n");
}

void link(node_t** work, node_t** current)
{
    if ((*work)->parent)
    {
        if ((*work)->parent->left == *work)
        {
            (*work)->parent->left = *current;
        }
        else if ((*work)->parent->right == *work)
        {
            (*work)->parent->right = *current;
        }
        else
        {
            printf("Link Error\n");
            exit(0);
        }
        (*current)->parent = (*work)->parent;
    }
}

node_t* build_parse_tree(buffer_t* input)
{
    struct node_t* work = malloc(sizeof(node_t));
    
    int i;
    for (i = 0; i < input->count; i++)
    {
        int j;
        for (j = 0; j < input->rule[i].size; j++)
        {
            struct node_t* current = malloc(sizeof(node_t));
            current->type = input->rule[i].list[j].type;
            current->symbol = input->rule[i].list[j].symbol;
            current->prior = default_prior[current->type];
            
            switch (current->type)
            {
                case NT_CHAR: case NT_END: case NT_EPS:
                    link(&work, &current);
                    
                    work = current;
                    break;
                    
                case NT_LPAREN:
                    link(&work, &current);
                    
                    work = current;
                    work->left = malloc(sizeof(node_t));
                    work->left->parent = work;
                    work = work->left;
                    break;
                    
                case NT_RPAREN:
                    while (work->type != NT_LPAREN || work->prior == 0)
                    {
                        work = work->parent;
                    }
                    work->prior = 0;
                    break;
                    
                case NT_STAR:
                    link(&work, &current);
                    
                    current->left = work;
                    work->parent = current;
                    work = current;
                    break;
                    
                case NT_OR: case NT_CAT:
                    while (work->parent && work->parent->prior <= current->prior)
                    {
                        work = work->parent;
                    }
                    link(&work, &current);
                    
                    current->left = work;
                    current->right = malloc(sizeof(node_t));
                    current->right->parent = current;
                    work->parent = current;
                    work = current->right;
            }
        }
        
        while (work->parent)
        {
            work = work->parent;
        }
        
        if (i != input->count - 1)
        {
            struct node_t* current = malloc(sizeof(node_t));
            current->type = NT_OR;
            current->prior = default_prior[current->type];
            
            current->left = work;
            current->right = malloc(sizeof(node_t));
            current->right->parent = current;
            work->parent = current;
            work = current->right;
        }
    }
    
    return work;
}


void merge_arrays(node_t* a[1000], int* sz_a, node_t* b[1000], int* sz_b)
{
    int i;
    for (i = 0; i < *sz_b; i++)
    {
        a[(*sz_a)++] = b[i];
    }
}

void merge_sets(node_t* a[1000], int* sz_a, node_t* b[1000], int* sz_b)
{
    int i;
    for (i = 0; i < *sz_b; i++)
    {
        bool found = false;
        int j;
        for (j = 0; j < *sz_a; j++)
        {
            if (a[j] == b[i])
            {
                found = true;
                break;
            }
        }
        
        if (!found)
        {
            a[(*sz_a)++] = b[i];
        }
    }
}

void calc_sets(node_t** v)
{
    if ((*v)->left)
    {
        calc_sets(&(*v)->left);
    }
    if ((*v)->right)
    {
        calc_sets(&(*v)->right);
    }
    if ((*v)->type == NT_CHAR || (*v)->type == NT_END)
    {
        (*v)->is_nullable = false;
        (*v)->first[(*v)->first_ptr++] = (*v)->last[(*v)->last_ptr++] = *v;
    }
    else if ((*v)->type == NT_OR)
    {
        (*v)->is_nullable = (*v)->left->is_nullable || (*v)->right->is_nullable;
        merge_arrays((*v)->first, &(*v)->first_ptr, (*v)->left->first, &(*v)->left->first_ptr);
        merge_arrays((*v)->first, &(*v)->first_ptr, (*v)->right->first, &(*v)->right->first_ptr);

        merge_arrays((*v)->last, &(*v)->last_ptr, (*v)->left->last, &(*v)->left->last_ptr);
        merge_arrays((*v)->last, &(*v)->last_ptr, (*v)->right->last, &(*v)->right->last_ptr);
    }
    else if ((*v)->type == NT_CAT)
    {
        (*v)->is_nullable = (*v)->left->is_nullable && (*v)->right->is_nullable;
        merge_arrays((*v)->first, &(*v)->first_ptr, (*v)->left->first, &(*v)->left->first_ptr);
        if ((*v)->left->is_nullable)
        {
            merge_arrays((*v)->first, &(*v)->first_ptr, (*v)->right->first, &(*v)->right->first_ptr);
        }
        
        merge_arrays((*v)->last, &(*v)->last_ptr, (*v)->right->last, &(*v)->right->last_ptr);
        if ((*v)->right->is_nullable)
        {
            merge_arrays((*v)->last, &(*v)->last_ptr, (*v)->left->last, &(*v)->left->last_ptr);
        }
        
        int i;
        for (i = 0; i < (*v)->left->last_ptr; i++)
        {
            merge_sets((*v)->left->last[i]->follow, &(*v)->left->last[i]->follow_ptr, (*v)->right->first, &(*v)->right->first_ptr);
        }
    }
    else if ((*v)->type == NT_STAR)
    {
        (*v)->is_nullable = true;
        merge_arrays((*v)->first, &(*v)->first_ptr, (*v)->left->first, &(*v)->left->first_ptr);
        merge_arrays((*v)->last, &(*v)->last_ptr, (*v)->left->last, &(*v)->left->last_ptr);
        
        int i;
        for (i = 0; i < (*v)->left->last_ptr; i++)
        {
            merge_sets((*v)->left->last[i]->follow, &(*v)->left->last[i]->follow_ptr, (*v)->left->first, &(*v)->left->first_ptr);
        }
    }
    else if ((*v)->type == NT_LPAREN)
    {
        (*v)->is_nullable = (*v)->left->is_nullable;
        merge_arrays((*v)->first, &(*v)->first_ptr, (*v)->left->first, &(*v)->left->first_ptr);
        merge_arrays((*v)->last, &(*v)->last_ptr, (*v)->left->last, &(*v)->left->last_ptr);
    }
    else if ((*v)->type == NT_EPS)
    {
        (*v)->is_nullable = true;
    }
}


int find_state(node_t* aux[N][N], int size, int state_size[N])
{
    int i;
    for (i = 0; i < size; i++)
    {
        if (state_size[i] != state_size[size])
        {
            continue;
        }
        
        bool is_same = true;
        int j;
        bool used[N] = {[0 ... N - 1] = false};
        for (j = 0; j < state_size[i]; j++)
        {
            int k;
            bool found = false;
            for (k = 0; k < state_size[size]; k++)
            {
                if (!used[k] && aux[i][j] == aux[size][k])
                {
                    used[k] = true;
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                is_same = false;
                break;
            }
        }
        if (is_same)
        {
            return i;
        }
    }
    
    return size;
}


void print_arr2(node_t* arr[(1 << CHAR_BIT)], int *sz)
{
    int i;
    for (i = 0; i < *sz; i++)
    {
        printf("%p ", arr[i]);
    }
    printf("\n");
}


void build_automaton(int autom[N][(1 << CHAR_BIT)], int* size, int final[N], node_t* root, 
    buffer_t* input)
{
    node_t* aux[N][N];
    int state_size[N];
    int i;
    for (i = 0; i < root->first_ptr; i++)
    {
        aux[0][state_size[0]++] = root->first[i];
    }
    (*size)++;
    
    
    for (i = 0; i < *size; i++)
    {
        int j;
        for (j = 0; j < state_size[i]; j++)
        {
            if (aux[i][j]->type == NT_END && (final[i] == NOT_FINAL || 
                input->rule[final[i]].prior < 
                    input->rule[aux[i][j]->symbol].prior))
            {
                final[i] = aux[i][j]->symbol;
            }
        }
        
        for (j = 0; j < (1 << CHAR_BIT); j++)
        {
            state_size[*size] = 0;
            int k;
            for (k = 0; k < state_size[i]; k++)
            {
                if (aux[i][k]->symbol == j)
                {
                    merge_sets(aux[*size], &state_size[*size], aux[i][k]->follow, &aux[i][k]->follow_ptr);
                    if (aux[i][k]->type == NT_EPS)
                    {
                        //printf("WTF???\n");
                    }
                }
            }
            if (state_size[*size] == 0)
            {
                if (j == NT_EPS)
                {
                    //printf("WTF???\n");
                }
                continue;
            }
            int state_idx = find_state(aux, *size, state_size);
            
            if (state_idx == *size)
            {
                (*size)++;
            }
            
            if (state_size[state_idx] == 0)
            {
                autom[i][j] = ERROR_STATE;
            }
            else
            {
                autom[i][j] = state_idx;
            }
        }
    }
    
    /*
    printf("%d\n", *size);
    for (i = 0; i < *size; i++)
    {
        printf("%d ", i);
        int j;
        for (j = 0; j < state_size[i]; j++)
        {
            printf("%p ", aux[i][j]);
        }
        printf("\n");
    }
    
    printf("aaaa\n");
    for (i = 0; i < *size; i++)
    {
        printf("%d ", i);
        int j;
        for (j = 0; j < (1 << CHAR_BIT); j++)
        {
            if (autom[i][j] != ERROR_STATE)
            {
                printf("(%d, %d) ", j, autom[i][j]);
            }
        }
        if (final[i] != NOT_FINAL)
        {
            printf("%s ", input->rule[final[i]].abbrev);
        }
        printf("\n");
    }
    */
}


void beautify_automaton(int autom[N][(1 << CHAR_BIT)], int* size, int final[N],
    buffer_t* input)
{
    printf(".size = %d,\n.final = (char*[])\n{\n    [0 ... %d] = \"\",\n", *size, *size - 1);
    int i;
    for (i = 0; i < *size; i++)
    {
        if (final[i] != NOT_FINAL)
        {
            printf("    [%d] = \"%s\",\n", i, input->rule[final[i]].abbrev);
        }
    }
    printf("},\n.table = (int*[])\n{\n");
    
    for (i = 0; i < *size; i++)
    {
        printf("    (int[])\n    {\n        [0 ... (1 << CHAR_BIT) - 1] = ERROR_STATE,\n");
        int j;
        for (j = 0; j < (1 << CHAR_BIT); j++)
        {
            if (autom[i][j] != ERROR_STATE)
            {
                printf("        [\'");
                if (j == '\n')
                {
                    printf("\\n");
                }
                else if (j == '\t')
                {
                    printf("\\t");
                }
                else if (j == '\r')
                {
                    printf("\\r");
                }
                else if (j == '\\')
                {
                    printf("\\\\");
                }
                else if (j == '\'')
                {
                    printf("\\'");
                    //printf("FUUUUUUUUUCCCKKKK\n");
                }
                else if (j == '\"')
                {
                    printf("\\\"");
                    //printf("FUUUUUUUUUCCCKKKK\n");
                }
                else
                {
                    printf("%c", j);
                }
                printf("\'] = %d,\n", autom[i][j]);
                //printf("        [\'%c\'] = %d,\n", j, autom[i][j]);
            }
        }
        printf("    },\n");
    }
    printf("},\n");
}
 

int main()
{
    buffer_t input = 
    {
        #include "lex_sequence.h"
    };
    

    node_t* work = build_parse_tree(&input);
    
    calc_sets(&work);
    
    //print_dfs(work);
    
    int autom[N][(1 << CHAR_BIT)];
    int i;
    for (i = 0; i < N; i++)
    {
        int j;
        for (j = 0; j < (1 << CHAR_BIT); j++)
        {
            autom[i][j] = ERROR_STATE;
        }
    }
    
    int size = 0;
    int final[N] = {[0 ... N - 1] = NOT_FINAL};
    build_automaton(autom, &size, final, work, &input);

    beautify_automaton(autom, &size, final, &input);
    
    return 0;
}