%{
#include <stdio.h>
#include "y.tab.h"
%}

%%
[' ']     ;
[0-9]+    {
              yylval = atoi(yytext);
              return NUMBER;
          }
[^0-9]    {             
              return yytext[0]; 
          }
%%
