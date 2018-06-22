#ifndef ERROR_H_
#define ERROR_H_

#include <stdarg.h>
#include "util.h"
#include <assert.h>

extern char linebuffer[500];
extern int pos;

typedef struct intList {int i; bool IsReport; struct intList *rest;} *IntList;

void error_newline(void);
void parse_error(string message, ...);
void type_error(int pos, string message, ...);
void error_reset(void);

#endif