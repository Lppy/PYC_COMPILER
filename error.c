#include "error.h"

bool anyErrors = FALSE;
static int lineNum = 1;
static IntList linePos = NULL;
// string fileName = "";

static IntList intList(int i, IntList rest){
	IntList l= checked_malloc(sizeof *l);
	l->i=i; l->rest=rest;
	return l;
}

void error_newline(void){
	lineNum++;
 	linePos = intList(pos, linePos);
}

void parse_error(int pos, string message, ...){
	va_list ap;
 	IntList lines = linePos;
 	int line_count = lineNum;

  	anyErrors=TRUE;
  	while (lines && lines->i >= pos){
  		lines=lines->rest; 
  		line_count--;
  	}

	if (lines){
		fprintf(stderr, "\nError at line % d: \n", line_count);
		fprintf(stderr, "     position % d: \n", pos - lines->i);
		if(*message)
			va_start(ap, message);
  			vfprintf(stderr, message, ap);
  			va_end(ap);
  			fprintf(stderr,"\n");
			fprintf(stderr, "%s\n", message);
	}

}

// void error_reset(string fname){
void error_reset(void){
	anyErrors=FALSE; 
	// fileName=fname; 
	lineNum=1;
	linePos=intList(0,NULL);
	// yyin = fopen(fname,"r");
	// if (!yyin){
	// 	parse_error(0, "cannot open file"); 
	// 	exit(1);
	// }
}

