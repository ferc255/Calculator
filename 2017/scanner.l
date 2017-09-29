%option never-interactive noyywrap bison-bridge
%{
#include <stdio.h>
#include <limits.h>
#include <stdbool.h>

#include "values.h"
%}

%%

[[:space:]]+	;
[[:digit:]]+  {
  *yylval = atoi (yytext);
  return (TERM_ID);
}
"+"	return (TERM_PLUS); 
"*"	return (TERM_MUL); 
"("	return (TERM_LPAREN); 
")"	return (TERM_RPAREN);
.       return (TERM_END);

%%
