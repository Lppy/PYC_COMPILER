%{
#include "y.tab.h"
#include "error.h"
#include <stdio.h>
#include <string.h>

void C_comment1();
void C_comment2();
void count();

int pos = 0;

%}

D			[0-9]
L			[a-zA-Z_]
O			[0-7]
H			[a-fA-F0-9]
E			[Ee][+-]?{D}+

%%

"/*"			{ count(); C_comment1(); }
"//"			{ count(); C_comment2(); }

"break"			{ count(); return BREAK; }
"case"			{ count(); return CASE; }
"char"			{ count(); return CHAR; }
"const"			{ count(); return CONST; }
"continue"		{ count(); return CONTINUE; }
"default"		{ count(); return DEFAULT; }
"do"			{ count(); return DO; }
"double"		{ count(); return DOUBLE; }
"else"			{ count(); return ELSE; }
"enum"			{ count(); return ENUM; }
"float"			{ count(); return FLOAT; }
"for"			{ count(); return FOR; }
"if"			{ count(); return IF; }
"int"			{ count(); return INT; }
"return"		{ count(); return RETURN; }
"short"			{ count(); return SHORT; }
"signed"		{ count(); return SIGNED; }
"static"		{ count(); return STATIC; }
"struct"		{ count(); return STRUCT; }
"switch"		{ count(); return SWITCH; }
"typedef"		{ count(); return TYPEDEF; }
"union"			{ count(); return UNION; }
"unsigned"		{ count(); return UNSIGNED; }
"void"			{ count(); return VOID; }
"while"			{ count(); return WHILE; }

"sizeof"		{ count(); return SIZEOF; }


{L}({L}|{D})*	{ count(); yylval.id = strdup(yytext); return IDENTIFIER; }

0[xX]{H}+			{ count(); int a; sscanf(yytext, "%x", &a); yylval.constant = L_Num(a); return CONSTANT; }
0{O}+				{ count(); int a; sscanf(yytext, "%o", &a); yylval.constant = L_Num(a); return CONSTANT; }
{D}+				{ count(); int a; sscanf(yytext, "%d", &a); yylval.constant = L_Num(a); return CONSTANT; }
{D}+"."{D}*			{ count(); double a; sscanf(yytext, "%lf", &a); yylval.constant = L_Real(a); return CONSTANT; }
'(\\.|[^\\'])+'		{ count(); char a = yytext[strlen(yytext)-2]; yylval.constant = L_Char(a); return CONSTANT; }
\"(\\.|[^\\"])*\"	{ count(); yytext[strlen(yytext)-1] = 0; yylval.id = strdup(yytext+1); return STRING_LITERAL; }

"..."			{ count(); return ELLIPSIS; }
">>="			{ count(); return RIGHT_ASSIGN; }
"<<="			{ count(); return LEFT_ASSIGN; }
"+="			{ count(); return ADD_ASSIGN; }
"-="			{ count(); return SUB_ASSIGN; }
"*="			{ count(); return MUL_ASSIGN; }
"/="			{ count(); return DIV_ASSIGN; }
"%="			{ count(); return MOD_ASSIGN; }
"&="			{ count(); return AND_ASSIGN; }
"^="			{ count(); return XOR_ASSIGN; }
"|="			{ count(); return OR_ASSIGN; }
">>"			{ count(); return RIGHT_OP; }
"<<"			{ count(); return LEFT_OP; }
"++"			{ count(); return INC_OP; }
"--"			{ count(); return DEC_OP; }
"->"			{ count(); return PTR_OP; }
"&&"			{ count(); return AND_OP; }
"||"			{ count(); return OR_OP; }
"<="			{ count(); return LE_OP; }
">="			{ count(); return GE_OP; }
"=="			{ count(); return EQ_OP; }
"!="			{ count(); return NE_OP; }
";"				{ count(); return ';'; }
"{"				{ count(); return '{'; }
"}"				{ count(); return '}'; }
","				{ count(); return ','; }
":"				{ count(); return ':'; }
"="				{ count(); return '='; }
"("				{ count(); return '('; }
")"				{ count(); return ')'; }
"["				{ count(); return '['; }
"]"				{ count(); return ']'; }
"."				{ count(); return '.'; }
"&"				{ count(); return '&'; }
"!"				{ count(); return '!'; }
"~"				{ count(); return '~'; }
"-"				{ count(); return '-'; }
"+"				{ count(); return '+'; }
"*"				{ count(); return '*'; }
"/"				{ count(); return '/'; }
"%"				{ count(); return '%'; }
"<"				{ count(); return '<'; }
">"				{ count(); return '>'; }
"^"				{ count(); return '^'; }
"|"				{ count(); return '|'; }
"?"				{ count(); return '?'; }
"\n"			{ count(); error_newline();}
[ \t\v\f]		{ count(); /* skip blank character */ }
.				{ count();  parse_error(pos, "unknown character");/* bad characters */ }

%%

int yywrap() //multiple text
{
	return 1;
}

void C_comment1()
{
	char c = getchar();
	while(1){
		while(c != '*' && c != 0) c = getchar();
		if(c == 0 || (c = getchar()) == '/') return;
	}
}

void C_comment2()
{
	char c = getchar();
	while(c != '\n' && c != 0) c = getchar();
}

void count()
{
	int i;
	for (i=0; yytext[i]!='\0'; i++)
		pos++;
}
