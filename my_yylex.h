#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "values.h"


int calc_max_match(lex_automaton_t* automaton, char* input, int start_pos, 
    token_id_t* token)
{
    int cur_state = 0;
    int result = 0;

    int i;
    for (i = start_pos; i < strlen(input); i++)
    {
        cur_state = automaton->table[cur_state][input[i]];
        if (cur_state == ERROR_STATE)
        {
            return result;
        }
        if (automaton->final[cur_state] != INVALID_TOKEN)
        {
            *token = automaton->final[cur_state];
            result = i - start_pos + 1;
        }
    }
    
    return result;
}


void print_token(char* abbrev, char* input, int start_pos, int len)
{
    printf("<%s, '", abbrev);
    
    int i;
    for (i = 0; i < len; i++)
    {
        char symbol = input[start_pos + i];
        if (symbol == 9)
        {
            printf("\\t");
        }
        else if (symbol == 10)
        {
            printf("\\n");
        }
        else if (symbol == 13)
        {
            printf("\\r");
        }
        else
        {
            printf("%c", symbol);
        }
    }
    
    printf("'>\n");
}


token_t my_yylex()
{
    lex_automaton_t automaton =
    {
        .size = 4,
        .final = (int[])
        {
            [0 ... 3] = INVALID_TOKEN,
            [1] = 3,
            [2] = 5,
            [3] = 4,
        },
        .table = (int*[])
        {
            (int[])
            {
                [0 ... (1 << CHAR_BIT) - 1] = ERROR_STATE,
                ['+'] = 1,
                ['-'] = 2,
                ['0'] = 3,
                ['1'] = 3,
                ['2'] = 3,
                ['3'] = 3,
                ['4'] = 3,
                ['5'] = 3,
                ['6'] = 3,
                ['7'] = 3,
                ['8'] = 3,
                ['9'] = 3,
            },
            (int[])
            {
                [0 ... (1 << CHAR_BIT) - 1] = ERROR_STATE,
            },
            (int[])
            {
                [0 ... (1 << CHAR_BIT) - 1] = ERROR_STATE,
            },
            (int[])
            {
                [0 ... (1 << CHAR_BIT) - 1] = ERROR_STATE,
                ['0'] = 3,
                ['1'] = 3,
                ['2'] = 3,
                ['3'] = 3,
                ['4'] = 3,
                ['5'] = 3,
                ['6'] = 3,
                ['7'] = 3,
                ['8'] = 3,
                ['9'] = 3,
            },
        },

        //#include "lex_automaton.h"
    }; 
    //printf("%d\n", automaton.size);
    
    static bool is_initialized = false;
    static char input[BUFFER_SIZE];
    if (!is_initialized)
    {
        char temp[BUFFER_SIZE];
        while (fgets(temp, sizeof temp, stdin))
        {
            strcat(input, temp);
        }
        
        is_initialized = true;
    }
    
    static int i = 0;
    for ( ; i < strlen(input); )
    {
        if (input[i] == ' ' || input[i] == '\t' || input[i] == '\n')
        {
            i++;
            continue;
        }
        
        token_t token;
        int max_match = calc_max_match(&automaton, input, i, &token.id);
        
        if (max_match == 0)
        {
            printf("[my_yylex.h] Error while parsing input ");
            printf("at %d position.\n", i);
            exit(0);
        }
        
        if (token.id == 4)
        {
            char* temp = malloc(max_match);
            memcpy(temp, input + i, max_match);
            token.data = atoi(temp);
        }
        
        i += max_match;
        
        return token;
    }
    
    token_t end = 
    {
        .id = 0,
    };
    
    return end;
}