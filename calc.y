%{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <complex.h>

#include "values.h"
#include "calc.lex.h"

%}

%name-prefix "calc_"
%file-prefix "calc"
%union 
{	
  value_t number;
};
%start str
%token <number> NUMBER
%left '+' '-'
%left '*' '/'
%type <number> expr
%%   
str:		expr '\n'
{
  printf ("%d\n", $1.number);
}
         	;
expr:    '(' expr ')'
         {
			 $$ = $2;
         }
         |
         expr '+' expr
         {
	   $$.number = $1.number + $3.number;
         }
         |
		 expr '-' expr
         {
	   $$.number = $1.number - $3.number;
         }
         |
		 expr '*' expr
         {
	   $$.number = $1.number * $3.number;
	 }
         |
		 expr '/' expr
         {
	   $$.number = $1.number / $3.number;
	 }
         |
		 NUMBER
         ;    
%%

		 int main(int argc, char* argv[])
	 {
	   calc_parse ();
	   return (EXIT_SUCCESS);
	 }
