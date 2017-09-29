YACC=bison
LEX=flex

all: myLR

myLR: myLR.o scanner.lex.o 

build_result.h: follow
	./$^ > $@

myLR.o: myLR.c values.h build_result.h

myLR.c: scanner.lex.h

follow: follow.c
	$(CC) $^ -o $@


scanner.lex.c: scanner.l
	$(LEX) -o $@ $<
	echo $(LEX)

scanner.lex.h: scanner.l
	$(LEX) --header-file=$@ -o $*.c $<	
