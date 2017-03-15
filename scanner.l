%{
#include <stdio.h>
#include "values.h"
%}

%%
[' ']	;
[0-9]+	return NUMBER;
[^0-9]	return c2t[yytext[0]];
%%
