all: myLR calc

CFLAGS += -O2 -Wall
YACC = bison
calc: calc.lex.o calc.tab.o

calc.lex.o: calc.lex.c calc.lex.h

calc.tab.o: calc.tab.c calc.tab.h

calc.lex.c: calc.l calc.tab.h
	$(LEX) $^

calc.lex.h: calc.l
	$(LEX) --header-file=$@ -o $*.c $<

calc.tab.c: calc.y
	$(YACC) $^

calc.tab.h: calc.y
	$(YACC) -d $^

