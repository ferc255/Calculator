#!/bin/bash
echo "cc parse_grammar.c -o PG"
cc parse_grammar.c -o PG
echo "grammar parsing..."
./PG < grammar.txt > grammar.h
echo ""

echo "cc gen_tables -o GT"
cc gen_tables.c -o GT
echo "syntax tables generation..."
./GT > syn_tables.h
echo ""

echo "cc parse_lex_rules.c -o PLR"
cc parse_lex_rules.c -o PLR
echo "generation lex-sequence..."
./PLR < lex_rules.txt > lex_sequence.h
echo ""

echo "cc build_lex_autom.c -o BLA"
cc build_lex_autom.c -o BLA
echo "automaton building..."
./BLA > lex_automaton.h
echo ""

echo "cc build_executions.c -o BE"
cc build_executions.c -o BE
echo "building grammar execution..."
./BE < grammar.txt > executions.h
echo ""

echo "cc main.c -lm -o main"
cc main.c -lm -o main

echo ""
echo "cleaning temporarily files..."
rm PG grammar.h GT syn_tables.h 
rm PLR lex_sequence.h BLA lex_automaton.h
rm BE executions.h
echo "_ ready for using _"
