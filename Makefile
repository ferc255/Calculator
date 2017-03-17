myLR: myLR.o scanner.lex.o

myLR.c: scanner.lex.h

scanner.lex.c: scanner.l
	$(LEX) -o $@ $<

scanner.lex.h: scanner.l
	$(LEX) --header-file=$@ -o $*.lex.c $<	
