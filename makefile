CC = gcc
sources = $(wildcard src/*.c)
objects = $(patsubst %.c,build/%.o,$(notdir ${sources})) build/y.tab.o

# parser.o: y.tab.h
# @yacc -d t.y  2>/dev/null

parser: $(objects)
	$(CC) -o $@ $(objects)

build/%.o: src/%.c include/%.h | build
	$(CC) -o $@ -c $< -Iinclude

build/y.tab.o: build/y.tab.c build/lex.yy.c
	$(CC) -o build/y.tab.o -c build/y.tab.c -Iinclude

build/y.tab.c: y_C_compiler_yacc.y | build
	yacc -o build/y.tab.c -d y_C_compiler_yacc.y 

build/lex.yy.c: l_C_compiler_lex.l | build
	lex -o build/lex.yy.c l_C_compiler_lex.l

build:
	mkdir $@
	
