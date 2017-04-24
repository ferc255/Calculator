YACC=bison
LEX=flex

all: myLR mycalc follow

mycalc: calc.tab.o scanner.lex.o
	$(CC) $^ $(LOADLIBES) $(LDLIBS) -o $@

calc.tab.o: calc.tab.c

.SUFFIXES: .y .tab.c

.y.tab.c:
	$(YACC) $<
	 
myLR: myLR.o scanner.lex.o

myLR.c: scanner.lex.h

scanner.lex.c: scanner.l
	$(LEX) -o $@ $<
	echo $(LEX)

scanner.lex.h: scanner.l
	$(LEX) --header-file=$@ -o $*.lex.c $<	

follow: follow.c
	$(CC) $^ -o $@
