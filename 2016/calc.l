%{
#include <stdio.h>
#include <complex.h>
#include "values.h"
#include "calc.tab.h"
%}

%option prefix="calc"

%%
[' ']                      ;

[0-9]+\.[0-9]+[i] | 
[0-9]+[i]              {
                           calctext[strlen(calctext) - 1] = 0;
                           calclval.number.type = VT_COMPLEX;
                           calclval.number.xldl = 0 + atof(calctext) * I;
                           return NUMBER;

                       }

[0-9]+\.[0-9]+         {
                           calclval.number.type = VT_FLOAT;
                           calclval.number.ldl = atof(calctext);
                           return NUMBER;
                       }

[0-9]+                 {
                           calclval.number.type = VT_INT;
                           calclval.number.llint = atoi(calctext);
                           return NUMBER;
                       }

[^0-9]                 {             
                           return calctext[0]; 
                       }
%%
