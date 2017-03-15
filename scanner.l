%{
#include <stdio.h>
#include "values.h"

#define YY_DECL state_t* yylex() 
state_t result;
%}

%%
[' ']	;
[0-9]+	{
		result.token = NUMBER;
		result.number = atoi(yytext);
		return &result;
	}
[^0-9]	{
		result.token = c2t[yytext[0]];
		return &result;
	}
%%
