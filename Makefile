program:  y.tab.o lex.yy.o
	gcc y.tab.o lex.yy.o -o calc
	./calc

y.tab.o: y.tab.c
	gcc -c y.tab.c

lex.yy.o: lex.yy.c  y.tab.h
	gcc -c lex.yy.c

y.tab.c: calc.yacc
	yacc -d calc.yacc

lex.yy.c: calc.lex
	lex calc.lex
