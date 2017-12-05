all: lex_app

lex_app: automaton.h my_yylex.c
	$(CC) my_yylex.c -o $@

automaton.h: BLA
	./BLA > automaton.h

BLA: lex_sequence.h build_lex_autom.c
	$(CC) build_lex_autom.c -o $@

lex_sequence.h: PLR lex_rules.txt
	./PLR < lex_rules.txt > lex_sequence.h

PLR: parse_lex_rules.c
	$(CC) $< -o $@
