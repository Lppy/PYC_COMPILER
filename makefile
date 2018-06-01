# CC = gcc
# OBJ = parser.o error.o until.o 

# make:$(OBJ)
# 	$(CC) -o parser $(OBJ)

# parser.o: y.tab.h

parser: y.tab.o error.o util.o
	gcc -o parser y.tab.o error.o util.o

y.tab.o: y.tab.c lex.yy.c
	gcc -c y.tab.c

error.o: error.c error.h
	gcc -c error.c

util.o: util.c
	gcc -c util.c

y.tab.c: t.y
	@yacc -d t.y

lex.yy.c: l_C_compiler_lex.l
	lex l_C_compiler_lex.l
