#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

void *checked_malloc(int len)
{
	void *p = malloc(len);
	if (!p) {
		fprintf(stderr,"\nRan out of memory!\n");
		exit(1);
	}
	return p;
}

string String(char *s)
{
	string p = checked_malloc(strlen(s)+1);
	strcpy(p,s);
	return p;
}

U_boolList U_BoolList(bool head, U_boolList tail)
{ 
	U_boolList list = checked_malloc(sizeof(*list));
	list->head = head;
	list->tail = tail;
	return list;
}

L_constant L_Num(int num)
{
	L_constant p = checked_malloc(sizeof(*p));
	p->kind = L_num;
	p->u.num = num;
	return p;
}

L_constant L_Real(double real)
{
	L_constant p = checked_malloc(sizeof(*p));
	p->kind = L_real;
	p->u.real = real;
	return p;
}

L_constant L_Char(char c)
{
	L_constant p = checked_malloc(sizeof(*p));
	p->kind = L_char;
	p->u.c = c;
	return p;
}