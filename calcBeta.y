%{
#include <stdlib.h>
#include <stdio.h>
%}

%name-prefix "calc"
%file-prefix "calc"



%union 
{
    int itg; 
    double dbl;
	struct
	{
		int type;
		int fint;
		double fdbl;
	} pair;
}

%start str

%token <itg> INTEGER
%token <dbl> FLOAT

%left '+' '-'
%left '*' '/'
%type <pair> expr
%type <pair> intNum
%type <pair> doubNum

%%   
str:     expr '\n'
         {
			 if($1.type == 0)
				 printf("%d\n", $1.fint);
			 else
				 printf("%f\n", $1.fdbl);
             return 1;
         }
         ;

expr:    expr '+' expr
         {
             if ($1.type == 0 && $3.type == 0)
			 {
				 $$.type = 0;
				 $$.fint = $1.fint + $3.fint;
				 $$.fdbl = (double) $$.fint;
			 }
			 else
			 {
				 $$.type = 1;
				 $$.fdbl = $1.fdbl + $3.fdbl;
			 }
         }
         |
         intNum
		 |
		 doubNum
         ;   

intNum:  INTEGER
         {
			 $$.type = 0;
			 $$.fint = $1;
			 $$.fdbl = (double) $$.fint;
		 }

doubNum: FLOAT
         {
			 $$.type = 1;
			 $$.fdbl = $1;
		 }    
%%


main(int argc, char* argv[])
{
	FILE *fp;
	if((fp = fopen("auxilary.txt", "w")) == NULL)
	{
		printf("couldn't open file for writing\n");
		return 0;
	}

	int i = 1;
	for (; i < argc; i++)
	{
		fprintf(fp, "%s ", argv[i]);
	}
	fprintf(fp, "\n");

	fclose(fp);

	if((fp = fopen("auxilary.txt", "r")) == NULL) 
	{
		printf("couldn't open temp for reading\n");
		return 0;
	}

	extern FILE *calcin;
	calcin=fp;
	yyparse();

	fclose(fp);
}

calcerror(s)
{
    printf("error\n");
}

calcwrap()
{
    return(1);
}
