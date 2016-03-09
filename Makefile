calculator: lex.calc.c calc.tab.h calc.tab.c
	gcc calc.tab.c lex.calc.c -o calc

lex.calc.c: calc.l
	$(LEX) $^

calc.tab.c: calc.y
	$(YACC) $^

calc.tab.h: calc.y
	$(YACC) -d $^
