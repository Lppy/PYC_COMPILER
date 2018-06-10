#include "error.h"

bool anyErrors = FALSE;
static int lineNum = 1;
static IntList linePos = NULL;
// string fileName = "";

static IntList intList(int i, IntList rest){
	IntList l= checked_malloc(sizeof *l);
	l->i=i; l->rest=rest;
	l->IsReport=FALSE;
	return l;
}

void error_newline(void){
	lineNum++;
 	linePos = intList(pos, linePos);
}

void parse_error(string message, ...){
	va_list ap;
 	IntList lines = linePos;
 	int line_count = lineNum;
 	int i;

  	anyErrors=TRUE;
  	while (lines && lines->i >= pos){
  		lines=lines->rest; 
  		line_count--;
  	}

	if (lines && (lines->IsReport)==FALSE){
		fprintf(stderr, "\nError at line % d: \n", line_count);
		fprintf(stderr, "%s\n", linebuffer);
		for(i=0; i<pos - lines->i; i++)fprintf(stderr, " ");
		fprintf(stderr, "^\n");
		lines->IsReport=TRUE;
		if(*message){
			va_start(ap, message);
  			vfprintf(stderr, message, ap);
  			va_end(ap);
  			fprintf(stderr,"\n");
  			// fprintf(stderr, "%s\n", message);
  		}	
	}
}

void type_error(int pos, string message, ...){
	va_list ap;
 	IntList lines = linePos;
 	int line_count = lineNum;
 	int i;

  	anyErrors=TRUE;
  	while (lines && lines->i >= pos){
  		lines=lines->rest; 
  		line_count--;
  	}

	if (lines && (lines->IsReport)==FALSE){
		fprintf(stderr, "\nError at line % d: \n", line_count);
		fprintf(stderr, "%s\n", linebuffer);
		for(i=0; i<pos - lines->i; i++)fprintf(stderr, " ");
		fprintf(stderr, "^\n");
		lines->IsReport=TRUE;
		if(*message){
			va_start(ap, message);
  			vfprintf(stderr, message, ap);
  			va_end(ap);
  			fprintf(stderr,"\n");
  			// fprintf(stderr, "%s\n", message);
  		}	
	}
	assert(0);
}

void error_reset(void){
	anyErrors=FALSE; 
	lineNum=1;
	linePos=intList(0,NULL);
}

