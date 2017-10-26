#!/bin/bash
echo "cc lex_part.c -o myflex"
cc parse_lex_rules.c -o aux_lex
echo "generation lex-sequence..."
./aux_lex < lex_rules.txt > lex_sequence.h

echo "cc build_autom.c -o mybison"
cc build_lex_autom.c -o aux_autom
echo "automaton building..."
./aux_autom > automaton.h
#cat automaton.h

echo "cc main.c -o task4"
cc my_yylex.c -o lex_app

echo ""
echo "cleaning temporarily files..."
rm lex_sequence.h automaton.h aux_autom aux_lex
echo "_ ready for using _"