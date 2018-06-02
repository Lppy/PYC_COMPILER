CC = gcc
objects = obj/y.tab.o obj/error.o obj/util.o obj/absyn.o obj/symbol.o obj/table.o

# make:$(OBJ)
# 	$(CC) -o parser $(OBJ)

# parser.o: y.tab.h
# @yacc -d t.y  2>/dev/null

parser: $(objects)
	gcc -o parser $(objects)

obj/y.tab.o: src/y.tab.c src/lex.yy.c | obj
	gcc -o obj/y.tab.o -c src/y.tab.c -Iinclude

obj/error.o: src/error.c include/error.h | obj
	gcc -o obj/error.o -c src/error.c -Iinclude

obj/absyn.o: src/absyn.c include/absyn.h | obj
	gcc -o obj/absyn.o -c src/absyn.c -Iinclude

obj/symbol.o: src/symbol.c include/symbol.h | obj
	gcc -o obj/symbol.o -c src/symbol.c -Iinclude

obj/table.o: src/table.c include/table.h | obj
	gcc -o obj/table.o -c src/table.c -Iinclude

obj/util.o: src/util.c include/util.h | obj
	gcc -o obj/util.o -c src/util.c -Iinclude

src/y.tab.c: y_C_compiler_yacc.y
	yacc -o src/y.tab.c -d y_C_compiler_yacc.y 

src/lex.yy.c: l_C_compiler_lex.l
	lex -o src/lex.yy.c l_C_compiler_lex.l

obj:
	mkdir $@
	
