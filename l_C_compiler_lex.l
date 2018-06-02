%{

#include "util.h"
#include "y.tab.h"

void count();

int pos = 0;

%}

D			[0-9]
L			[a-zA-Z_]
O			[0-7]
H			[a-fA-F0-9]
E			[Ee][+-]?{D}+

%%

"/*"([^*/]+[\*\/]?)*"*/"	{ count(); }
"//"[^\n]*\n				{ count(); }

"break"			{ count(); return BREAK; }
"case"			{ count(); return CASE; }
"char"			{ count(); return CHAR; }
"continue"		{ count(); return CONTINUE; }
"default"		{ count(); return DEFAULT; }
"do"			{ count(); return DO; }
"else"			{ count(); return ELSE; }
"float"			{ count(); return FLOAT; }
"for"			{ count(); return FOR; }
"if"			{ count(); return IF; }
"int"			{ count(); return INT; }
"return"		{ count(); return RETURN; }
"struct"		{ count(); return STRUCT; }
"switch"		{ count(); return SWITCH; }
"void"			{ count(); return VOID; }
"while"			{ count(); return WHILE; }


{L}({L}|{D})*	{ count(); yylval.id = strdup(yytext); return IDENTIFIER; }

0[xX]{H}+			{ count(); int a; sscanf(yytext, "%x", &a); yylval.constant = L_Num(a); return CONSTANT; }
0{O}+				{ count(); int a; sscanf(yytext, "%o", &a); yylval.constant = L_Num(a); return CONSTANT; }
{D}+				{ count(); int a; sscanf(yytext, "%d", &a); yylval.constant = L_Num(a); return CONSTANT; }
{D}+"."{D}*			{ count(); double a; sscanf(yytext, "%lf", &a); yylval.constant = L_Real(a); return CONSTANT; }
'(\\.|[^\\'])+'		{ count(); char a = yytext[strlen(yytext)-2]; yylval.constant = L_Char(a); return CONSTANT; }
\"(\\.|[^\\"])*\"	{ count(); yytext[strlen(yytext)-1] = 0; yylval.id = strdup(yytext+1); return STRING_LITERAL; }


">>"			{ count(); return RIGHT_OP; }
"<<"			{ count(); return LEFT_OP; }
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
"#"				{ return 0; }
.				{ count();  parse_error(pos, "unknown character");/* bad characters */ }

%%

int yywrap() //multiple text
{
	return 1;
}

void count()
{
	int i;
	for (i=0; yytext[i]!='\0'; i++)
		pos++;
}
