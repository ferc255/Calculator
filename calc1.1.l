%{
#include <stdio.h>
#include "calc.tab.h"
%}

%option prefix="calc"

%%
[' ']     ;
[0-9]+\.[0-9]+     {
                       calclval.dbl = atof(calctext);
                       return FLOAT;
                   }
[0-9]+             {
                       calclval.itg = atoi(calctext);
                       return INTEGER;
                   }
[^0-9]             {             
                       return calctext[0]; 
                   }
%%

