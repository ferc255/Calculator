#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef enum
{
    NT_CHAR,
    NT_CAT,
    NT_STAR,
    NT_OR,
    NT_END,
    NT_LPAREN,
    NT_RPAREN,
    NT_NULL,
} node_type_t;

typedef struct rule_token_t
{
    node_type_t type;
    char symbol;
} rule_token_t;

typedef struct buffer_t
{
    int size;
    rule_token_t* list;
} buffer_t;

typedef struct node_t
{
    node_type_t type;
    char symbol;
    int prior;
    struct node_t* left;
    struct node_t* right;
    struct node_t* parent;
} node_t;

int default_prior[7] =
{
    [NT_CHAR] = 0,
    [NT_END] = 0,
    [NT_STAR] = 1,
    [NT_CAT] = 2,
    [NT_OR] = 3,
    [NT_LPAREN] = 4,
    [NT_RPAREN] = 4,
};

/*buffer_t input = 
{
    .size = 14,
    .list = (rule_token_t[])
    {
        {
            .type = NT_LPAREN,
        },
        {
            .type = NT_CHAR,
            .symbol = 'a',
        },
        {
            .type = NT_OR,
        },
        {
            .type = NT_CHAR,
            .symbol = 'b',
        },
        {
            .type = NT_RPAREN,
        },
        {
            .type = NT_STAR,
        },
        {
            .type = NT_CAT,
        },
        {
            .type = NT_CHAR,
            .symbol = 'a',
        },
        {
            .type = NT_CAT,
        },
        {
            .type = NT_CHAR,
            .symbol = 'b',
        },
        {
            .type = NT_CAT,
        },
        {
            .type = NT_CHAR,
            .symbol = 'b',
        },
        {
            .type = NT_CAT,
        },
        {
            .type = NT_END,
        },
    },
};
*/

buffer_t input = 
{
    .size = 18,
    .list = (rule_token_t[])
    {
        {
            .type = NT_CHAR,
            .symbol = 'a',
        },
        {
            .type = NT_CAT,
        },
        {
            .type = NT_LPAREN,
        },
        {
            .type = NT_CHAR,
            .symbol = 'b',
        },
        {
            .type = NT_CAT,
        },
        {
            .type = NT_LPAREN,
        },
        {
            .type = NT_CHAR,
            .symbol = 'c',
        },
        {
            .type = NT_OR,
        },
        {
            .type = NT_CHAR,
            .symbol = 'd',
        },
        {
            .type = NT_STAR,
        },
        {
            .type = NT_RPAREN,
        },
        {
            .type = NT_CAT,
        },
        {
            .type = NT_CHAR,
            .symbol = 'e',
        },
        {
            .type = NT_RPAREN,
        },
        {
            .type = NT_CAT,
        },
        {
            .type = NT_CHAR,
            .symbol = 'f',
        },
        {
            .type = NT_CAT,
        },
        {
            .type = NT_END,
        },
    },
};

void dfs(node_t* v)
{
    if (v->type)
    {
        printf("[%d]\n", v->type);
    }
    else
    {
        printf("[%c]\n", v->symbol);
    }
    if (v->left)
    {
        printf("LF\n");
        dfs(v->left);
    }
    if (v->right)
    {
        printf("RG\n");
        dfs(v->right);
    }
    if (v->type)
    {
        printf("[%d] ", v->type);
    }
    else
    {
        printf("[%c] ", v->symbol);
    }
    printf("EXIT\n");
}

int main()
{
    /*
    node_t b;
    a->prior = 2;
    printf("%d\n", a->prior);
    */
    struct node_t* a = malloc(sizeof(node_t));
    struct node_t* b = malloc(sizeof(node_t));
    
    struct node_t* work = malloc(sizeof(node_t));
    work->type = NT_NULL;
    int i;
    for (i = 0; i < input.size; i++)
    {
        struct node_t* current = malloc(sizeof(node_t));
        current->type = input.list[i].type;
        current->symbol = input.list[i].symbol;
        current->prior = default_prior[current->type];
        
        if (current->symbol == 'f')
            printf("OK      dasfdafa sasdf asdf a\n");
        switch (current->type)
        {
            case NT_CHAR: case NT_END:
                if (work->parent)
                {
                    if (work->parent->left == work)
                    {
                        work->parent->left = current;
                    }
                    else if (work->parent->right == work)
                    {
                        work->parent->right = current;
                    }
                    else
                    {
                        printf("Something wrong at %d-th symbol\n", i);
                        return 0;
                    }
                    current->parent = work->parent;
                }
                work = current;
                break;
            case NT_LPAREN:
                if (work->parent)
                {
                    if (work->parent->left == work)
                    {
                        work->parent->left = current;
                    }
                    else if (work->parent->right == work)
                    {
                        work->parent->right = current;
                    }
                    else
                    {
                        printf("Something wrong at %d-th symbol\n", i);
                        return 0;
                    }
                    current->parent = work->parent;
                }
                printf("type %d\n", work->type);
                work = current;
                work->left = malloc(sizeof(node_t));
                work->left->parent = work;
                work = work->left;
                work->type = NT_NULL;
                break;
            case NT_RPAREN:
                while (work->type != NT_LPAREN)
                {
                    work = work->parent;
                }
                work->prior = 0;
                break;
            case NT_STAR:
                if (work->parent)
                {
                    if (work->parent->left == work)
                    {
                        work->parent->left = current;
                    }
                    else if (work->parent->right == work)
                    {
                        work->parent->right = current;
                    }
                    else
                    {
                        printf("Something wrong at %d-th symbol\n", i);
                        return 0;
                    }
                    current->parent = work->parent;
                }
                current->left = work;
                work->parent = current;
                work = current;
                break;
            default:
                while (work->parent && work->parent->prior <= current->prior)
                {
                    work = work->parent;
                }
                if (work->parent)
                {
                    if (work->parent->left == work)
                    {
                        work->parent->left = current;
                    }
                    else if (work->parent->right == work)
                    {
                        work->parent->right = current;
                    }
                    else
                    {
                        printf("Something wrong at %d-th symbol\n", i);
                        return 0;
                    }
                    //printf("%d\n", work->type); 
                    current->parent = work->parent;
                }
                current->left = work;
                work->parent = current;
                current->right = malloc(sizeof(node_t));
                current->right->parent = current;
                work = current->right;
                work->type = NT_NULL;
        }
        //printf("---------------- %d\n", i);
        //dfs(work);
    }
    
    printf("=============%d OK\n", i);
    while (work->parent)
    {
        work = work->parent;
    }
    dfs(work);
    
    return 0;
}