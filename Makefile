all: lex_app

lex_app: automaton lex_app1 

lex_app1: my_yylex.c
	$(CC) $< -o $@
	
automaton: automaton.h building
	build1 > automaton.h
	
	
building: sequence build1 
	
build1: build_lex_autom.c
	$(CC) $< -o $@
	
sequence: lowest lex_sequence.h 
	lowest < lex_rules.txt > lex_sequence.h




lowest: parse_lex_rules
	$(CC) $< -o $@




tree: tree.c
	$(CC) $< -o $@