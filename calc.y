%{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <complex.h>

#include "values.h"
#include "scanner.lex.h"
#define yyerror
%}
%define api.pure full
%start str
%left PLUS MINUS
%token LPAREN RPAREN END
%token NUMBER

%%   
str:	expr END	
{
  printf ("%d\n", $1);
}
         	;
expr:    LPAREN expr RPAREN { $$ = $2; }
         | expr PLUS expr { $$ = $1 + $3; }
         | expr MINUS expr { $$ = $1 - $3; }
         | NUMBER
         ;    
%%

	 int main(int argc, char* argv[])
	 {
	   yyparse ();
	   return (EXIT_SUCCESS);
	 }
