%{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
%}

%name-prefix "calc"
%file-prefix "calc"

%start str

%token NUMBER

%left '+' '-'
%left '*' '/'

%%   
str:     expr '\n'
         {
             printf("%d\n", $1);
             return 1;
         }
         ;

expr:    '(' expr ')'
         {
             $$ = $2;
         }
         |
         expr '+' expr
         {
             $$ = $1 + $3;
         }
         |
         expr '-' expr
         {
             $$ = $1 - $3;
         }
         |
         expr '*' expr
         {
             $$ = $1 * $3;
         }
         |
         expr '/' expr
         {
             $$ = $1 / $3;
         }
         |
         NUMBER
         ;       
%%

extern struct calc_buffer_state* calc_scan_bytes(char * str, int len);

main(int argc, char* argv[])
{
	char input[100] = "";
	int totalLen = 0;
	int i;
	for (i = 1; i < argc; i++)
	{
		strcat(input, argv[i]);
		totalLen += strlen(argv[i]);
	}
	input[totalLen++] = '\n';

    struct calc_buffer_state* buffer = calc_scan_bytes(input, totalLen);
	calcparse();
}

calcerror(s)
{
    printf("error\n");
}

calcwrap()
{
    return(1);
}
