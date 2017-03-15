mys: myLR.c values.h lex.yy.c
	gcc myLR.c lex.yy.c -ll -o mys 

lex.yy.c: scanner.l
	$(LEX) scanner.l

