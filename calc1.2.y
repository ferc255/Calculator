%{
#include <stdlib.h>
#include <stdio.h>
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


main(int argc, char* argv[])
{
	FILE *fp;
	if((fp = fopen("auxilary.txt", "w")) == NULL)
	{
		printf("couldn't open file for writing\n");
		return 0;
	}

	int i = 1;
	for (; i < argc; i++)
	{
		fprintf(fp, "%s ", argv[i]);
	}
	fprintf(fp, "\n");

	fclose(fp);

	if((fp = fopen("auxilary.txt", "r")) == NULL) 
	{
		printf("couldn't open temp for reading\n");
		return 0;
	}

	extern FILE *calcin;
	calcin=fp;
	yyparse();

	fclose(fp);
}

calcerror(s)
{
    printf("error\n");
}

calcwrap()
{
    return(1);
}
