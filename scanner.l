%option never-interactive noyywrap bison-bridge
%{
#include <stdio.h>

#include "values.h"
%}

%%

[[:space:]]+	;
[[:digit:]]+  {
  *yylval = atoi (yytext);
  return (NUMBER);
}
"+"	return (PLUS); 
"-"	return (MINUS); 
"*"	return (MUL); 
"/"	return (DIV);
"("	return (LPAREN); 
")"	return (RPAREN);
.       return (END);

%%
