# CC = gcc
# OBJ = parser.o error.o until.o 

# make:$(OBJ)
# 	$(CC) -o parser $(OBJ)

# parser.o: y.tab.h
# @yacc -d t.y  2>/dev/null

parser: y.tab.o error.o util.o absyn.o symbol.o table.o
	gcc -o parser y.tab.o error.o util.o absyn.o symbol.o table.o

y.tab.o: y.tab.c lex.yy.c
	gcc -c y.tab.c

error.o: error.c error.h
	gcc -c error.c

absyn.o: absyn.c absyn.h
	gcc -c absyn.c

symbol.o: symbol.c symbol.h
	gcc -c symbol.c

table.o: table.c table.h
	gcc -c table.c

util.o: util.c
	gcc -c util.c

y.tab.c: y_C_compiler_yacc.y
	yacc -d y_C_compiler_yacc.y 

lex.yy.c: l_C_compiler_lex.l
	lex l_C_compiler_lex.l
	
