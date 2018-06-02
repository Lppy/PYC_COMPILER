#ifndef ERROR_H_
#define ERROR_H_

#include <stdarg.h>
#include "util.h"

extern int pos;
// extern FILE *yyin;

typedef struct intList {int i; struct intList *rest;} *IntList;

void error_newline(void);
void parse_error(int pos, string message, ...);
void error_reset(void);
// void error_reset(string fname);

#endif