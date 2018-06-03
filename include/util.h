#ifndef UTIL_H_
#define UTIL_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

typedef char* string;
typedef char bool;

#define TRUE 1
#define FALSE 0
#define null NULL

typedef struct L_constant_ *L_constant;
struct L_constant_ {
	enum {L_num, L_real, L_char} kind;
	union {
		int num;
		double real;
		char c;
	} u;
};
L_constant L_Num(int num);
L_constant L_Real(double real);
L_constant L_Char(char c);

void *checked_malloc(int);

string String(char *);

typedef struct U_boolList_ *U_boolList;
struct U_boolList_ {bool head; U_boolList tail;};
U_boolList U_BoolList(bool head, U_boolList tail);

#endif