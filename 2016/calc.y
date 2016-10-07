%{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <complex.h>
#include "values.h"
#define max(a, b) ((a > b) ? a : b)
void cast_value(value_t* value, value_type_t target)
{
	switch (target)
	{
		case VT_FLOAT:
			switch (value->type)
			{
				case VT_INT:
					value->ldl = value->llint;
					break;
			}
			break;
		case VT_COMPLEX:
			switch (value->type)
			{
				case VT_INT:
					value->xldl = value->llint + 0 * I;
					break;
				case VT_FLOAT:
					value->xldl = value->ldl + 0 * I;
					break;
			}
	}
	value->type = target;
}
%}
%name-prefix "calc"
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
				switch ($1.type)
				{
			 		case VT_INT:
						printf("%lld\n", $1.llint);
						break;
			 		case VT_FLOAT:
						printf("%Lf\n", $1.ldl);
						break;
					case VT_COMPLEX:
						if (cimagl($1.xldl) >= 0.0)
						{
							printf("%Lf + %Lfi\n", creall($1.xldl), cimagl($1.xldl));
						}
						else
						{
							printf("%Lf - %Lfi\n", creall($1.xldl), -cimagl($1.xldl));
						}
						break;
				}
			}
         	;
expr:    '(' expr ')'
         {
			 $$ = $2;
         }
         |
         expr '+' expr
         {
			 value_type_t max_type = max($1.type, $3.type);
			 $$.type = max_type;
			 cast_value(&$1, max_type);
			 cast_value(&$3, max_type);
			 switch (max_type)
			 {
			 	case VT_INT:
					 $$.llint = $1.llint + $3.llint;
					 break;
			 	case VT_FLOAT:
					 $$.ldl = $1.ldl + $3.ldl;
					 break;
			 	case VT_COMPLEX:
					$$.xldl = $1.xldl + $3.xldl;
					break;
			 }
         }
         |
		 expr '-' expr
         {
			 value_type_t max_type = max($1.type, $3.type);
			 $$.type = max_type;
			 cast_value(&$1, max_type);
			 cast_value(&$3, max_type);
			 switch (max_type)
			 {
			 	case VT_INT:
					 $$.llint = $1.llint - $3.llint;
					 break;
			 	case VT_FLOAT:
					 $$.ldl = $1.ldl - $3.ldl;
					 break;
			 	case VT_COMPLEX:
					$$.xldl = $1.xldl - $3.xldl;
					break;
			 }
         }
         |
		 expr '*' expr
         {
			 value_type_t max_type = max($1.type, $3.type);
			 $$.type = max_type;
			 cast_value(&$1, max_type);
			 cast_value(&$3, max_type);
			 switch (max_type)
			 {
			 	case VT_INT:
					 $$.llint = $1.llint * $3.llint;
					 break;
			 	case VT_FLOAT:
					 $$.ldl = $1.ldl * $3.ldl;
					 break;
			 	case VT_COMPLEX:
					$$.xldl = $1.xldl * $3.xldl;
					break;
			 }
         }
         |
		 expr '/' expr
         {
			 value_type_t max_type = max($1.type, $3.type);
			 $$.type = max_type;
			 cast_value(&$1, max_type);
			 cast_value(&$3, max_type);
			 switch (max_type)
			 {
			 	case VT_INT:
					if ($3.llint != 0)
					{
						$$.llint = $1.llint / $3.llint;
					}
					else
					{
						calcerror();
						return 1;
					}
					break;
			 	case VT_FLOAT:
					if ($3.ldl != 0.0)
					{
						$$.ldl = $1.ldl / $3.ldl;
					}
					else
					{
						calcerror();
						return 1;
					}
					break;
			 	case VT_COMPLEX:
					if (creall($3.xldl) != 0.0 || cimagl($3.xldl) != 0.0)
					{
						$$.xldl = $1.xldl / $3.xldl;
					}
					else
					{
						calcerror();
						return 1;
					}
					break;
			 }
         }
         |
		 NUMBER
         ;    
%%
extern struct calc_buffer_state* calc_scan_string(char* str);
main(int argc, char* argv[])
{
	int buf_size = 2;
	int i;
	for (i = 1; i < argc; i++)
	{
		buf_size += strlen(argv[i]);
	}
	char buf[buf_size];
	char* ptr = buf;
	for (i = 1; i < argc; ++i)
	{
		char* arg;
		for (arg = argv[i]; *arg; )
			*ptr++ = *arg++;
	}
	*ptr++ = '\n';
	*ptr = 0;
	struct calc_buffer_state* buffer = calc_scan_string(buf);
	calcparse();
}
calcerror(s)
{
	printf("error\n");
}
calcwrap()
{
	return(1);
}
