#include <stdbool.h>
#include <stdio.h>
#include <string.h>

const int BUFFER_SIZE = 1000;
const char NOT_CHAR = 254;

void print_token(char* type, char symbol)
{
    if (type == "NT_END")
    {
        printf("            {\n                .type = NT_END,\n");
        printf("                .symbol = %d,\n", symbol);
    }
    else
    {
        printf("            {\n                .type = %s,\n", type);
        
        if (symbol != NOT_CHAR)
        {
            printf("                .symbol = \'");
            if (symbol == '\n')
            {
                printf("\\n");
            }
            else if (symbol == '\t')
            {
                printf("\\t");
            }
            else if (symbol == '\r')
            {
                printf("\\r");
            }
            else if (symbol == '\\')
            {
                printf("\\\\");
            }
            else if (symbol == '\'')
            {
                printf("\\'");
            }
            else if (symbol == '\"')
            {
                printf("\\\"");
            }
            else
            {
                printf("%c", symbol);
            }
            printf("\',\n");
        }
    }
    
    printf("            },\n");
}

bool dont_need_cat(char* last, char* cur)
{
    if (last == "NT_OR" || last == "NT_LPAREN")
    {
        return true;
    }
    if (cur == "NT_OR" || cur == "NT_RPAREN" || cur == "NT_STAR")
    {
        return true;
    }
    return false;
}

void parse_rule(char input[BUFFER_SIZE], int idx)
{
    int first_semic = 0;
    int second_semic = 0;
    int i;
    for (i = 0; second_semic == 0; i++)
    {
        if (input[i] == ':')
        {
            if (first_semic == 0)
            {
                first_semic = i;
            }
            else
            {
                second_semic = i;
            }
        }
    }
    
    char s[BUFFER_SIZE];
    int s_ptr = 0;
    for (i = second_semic + 1; i < strlen(input); )
    {
        if (input[i] == '\\')
        {
            if (input[i + 1] == 'd')
            {
                s[s_ptr++] = '(';
                char j;
                for (j = '0'; j < '9'; j++)
                {
                    s[s_ptr++] = j;
                    s[s_ptr++] = '|';
                }
                s[s_ptr++] = '9';
                s[s_ptr++] = ')';
                
                i += 2;
            }
            else if (input[i + 1] == 'w')
            {
                s[s_ptr++] = '(';
                char j;
                for (j = 'a'; j <= 'z'; j++)
                {
                    s[s_ptr++] = j;
                    s[s_ptr++] = '|';
                }
                for (j = 'A'; j < 'Z'; j++)
                {
                    s[s_ptr++] = j;
                    s[s_ptr++] = '|';
                }
                s[s_ptr++] = 'Z';
                s[s_ptr++] = ')';
                
                i += 2;
            }
            else if (input[i + 1] == 's')
            {
                s[s_ptr++] = '(';
                s[s_ptr++] = ' ';
                s[s_ptr++] = '|';
                s[s_ptr++] = '\\';
                s[s_ptr++] = 'n';
                s[s_ptr++] = '|';
                s[s_ptr++] = '\\';
                s[s_ptr++] = 't';
                s[s_ptr++] = '|';
                s[s_ptr++] = '\\';
                s[s_ptr++] = 'r';
                s[s_ptr++] = ')';
                
                i += 2;
            }
            else
            {
                s[s_ptr++] = input[i++];
            }
        }
        else
        {
            s[s_ptr++] = input[i++];
        }
    }
    s[s_ptr - 1] = '\0';
    //printf("%s", s);
    
    char temp[BUFFER_SIZE];
    for (i = 0; i < first_semic; i++)
    {
        temp[i] = input[i];
    }
    temp[i] = '\0';
    

    printf("    {\n        .abbrev = \"%s\",\n", temp);
    
    for (i = first_semic + 1; i < second_semic; i++)
    {
        temp[i - first_semic - 1] = input[i];
    }
    temp[i - first_semic - 1] = '\0';
    
    printf("        .prior = %d,\n        .list = (rule_token_t[])\n        {\n", 
        atoi(temp));
    
    char* last = "NT_LPAREN";
    print_token(last, NOT_CHAR);
    
    //printf("%s\n", s);
    
    int size = 4;
    for (i = 0; i < strlen(s); )
    {
        size++;
        char* cur = "";
        char symbol = NOT_CHAR;
        if (s[i] == '\\')
        {
            if (s[i + 1] == 'e')
            {
                cur = "NT_EPS";
            }
            else if (s[i + 1] == 'n')
            {
                cur = "NT_CHAR";
                symbol = '\n';
            }
            else if (s[i + 1] == 't')
            {
                cur = "NT_CHAR";
                symbol = '\t';
            }
            else if (s[i + 1] == 'r')
            {
                cur = "NT_CHAR";
                symbol = '\r';
            }
            else
            {
                cur = "NT_CHAR";
                symbol = s[i + 1];
                //printf("%d\n", s[i + 1]);
            }
            
            i += 2;
        }
        else
        {
            if (s[i] == '|')
            {
                cur = "NT_OR";
            }
            else if (s[i] == '(')
            {
                cur = "NT_LPAREN";
            }
            else if (s[i] == ')')
            {
                cur = "NT_RPAREN";
            }
            else if (s[i] == '*')
            {
                cur = "NT_STAR";
            }
            else
            {
                cur = "NT_CHAR";
                symbol = s[i];
            }
            
            i++;
        }
        
        if (!dont_need_cat(last, cur))
        {
            size++;
            print_token("NT_CAT", NOT_CHAR);
        }
        print_token(cur, symbol);
        
        last = cur;
    }
    
    print_token("NT_RPAREN", NOT_CHAR);
    print_token("NT_CAT", NOT_CHAR);
    print_token("NT_END", idx);
    
    printf("        },\n        .size = %d,\n    },\n", size);
    
}

int main()
{
    char input[BUFFER_SIZE];
    char temp[BUFFER_SIZE];
    int count = 0;
    printf(".rule = (lex_rule_t[])\n{\n");
    while (fgets(temp, sizeof temp, stdin))
    {
        strcat(input, temp);
        parse_rule(temp, count);
        count++;
        //printf("%s\n", temp);
    }
    
    printf("},\n.count = %d,\n", count);
}