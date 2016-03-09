%{
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
             main();
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

main()
{
    return(calcparse());
}

calcerror(s)
{
    printf("error\n");
}

calcwrap()
{
    return(1);
}
