%{
#include <stdio.h>
%}

%start str

%token NUMBER

%left '+' '-'

%%   
str:     expr '\n'
         {
			 printf("%d\n", $1);
			 main();
         }
         ;

expr:    expr '+' expr
         {
             $$ = $1 + $3;
         }
         |
         expr '-' expr
		 {
			 $$ = $1 - $3;
		 }
         |
         NUMBER
         ;      
%%

main()
{
    return(yyparse());
}

yyerror(s)
{
    printf("error\n");
}

yywrap()
{
    return(1);
}
