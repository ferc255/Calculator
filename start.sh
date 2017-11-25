#!/bin/bash
echo "cc lex_part.c -o PLR"
cc parse_lex_rules.c -o PLR
echo "generation lex-sequence..."
./PLR < lex_rules.txt > lex_sequence.h

echo "cc build_autom.c -o BLA"
cc build_lex_autom.c -o BLA
echo "automaton building..."
./BLA > lex_automaton.h

echo "cc main.c -o lex_app"
cc my_yylex.c -o lex_app

echo ""
echo "cleaning temporarily files..."
rm PLR lex_sequence.h BLA lex_automaton.h
echo "_ ready for using _"