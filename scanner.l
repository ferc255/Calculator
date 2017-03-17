%option never-interactive noyywrap bison-bridge
%{
#include <stdio.h>
#include <limits.h>

#include "values.h"

static int c2t[1 << CHAR_BIT] = 
{
  [0 ... (1 << CHAR_BIT) - 1] = END,
  ['+'] = PLUS,
  ['*'] = MUL,
  ['('] = LPAREN,
  [')'] = RPAREN,
  ['-'] = MINUS,
  ['/'] = DIV,
};

%}

%%

[' ']	;
[0-9]+	{
		*yylval = atoi (yytext);
		return (NUMBER);
	}
[^0-9]	{
		return (c2t[yytext[0]]);
	}
%%
