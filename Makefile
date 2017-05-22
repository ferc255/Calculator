YACC=bison
LEX=flex

all: myLR

myLR: myLR.o scanner.lex.o 

build_result.h: follow
	./$^ > $@

myLR.c: scanner.lex.h build_result.h

scanner.lex.c: scanner.l
	$(LEX) -o $@ $<
	echo $(LEX)

scanner.lex.h: scanner.l
	$(LEX) --header-file=$@ -o $*.c $<	

follow: follow.c
	$(CC) $^ -o $@
