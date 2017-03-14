%{
#include <stdio.h>
#include <stdlib.h>
#include "values.h"
#include "calc.tab.h"
%}

%option prefix="calc"

%%
[' ']                      ;

[0-9]+                 {
                           calc_lval.number.number = atoi (calctext);
                           return NUMBER;
                       }

[^0-9]                 {             
  return c2t[(int)calctext[0]]; 
                       }
%%
