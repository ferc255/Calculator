#include <stdio.h>
#include <stdlib.h>

typedef enum
{
    CHAR,
    CAT,
    STAR,
    OR,
    END
} node_type_t;

typedef struct node_t
{
    node_type_t type;
    char symbol;
    int prior;
    struct node_t* left;
    struct node_t* right;
} node_t;

int main()
{
    node_t b;
    struct node_t* a = malloc(sizeof(node_t));
    a->prior = 2;
    printf("%d\n", a->prior);
    
    return 0;
}