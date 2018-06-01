%{
//#define YYSTYPE char*
#include <stdio.h>
#include "util.h"
#include "error.h"
#include "lex.yy.c"
int yyerror(string message);
%}
%union {char* id; L_constant constant;}
%token<constant> CONSTANT
%token<id> IDENTIFIER STRING_LITERAL SIZEOF
%token PTR_OP INC_OP DEC_OP LEFT_OP RIGHT_OP LE_OP GE_OP EQ_OP NE_OP
%token AND_OP OR_OP MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN ADD_ASSIGN
%token SUB_ASSIGN LEFT_ASSIGN RIGHT_ASSIGN AND_ASSIGN
%token XOR_ASSIGN OR_ASSIGN TYPE_NAME

%token TYPEDEF STATIC
%token CHAR SHORT INT SIGNED UNSIGNED FLOAT DOUBLE CONST VOID
%token STRUCT UNION ENUM ELLIPSIS

%token CASE DEFAULT IF ELSE SWITCH WHILE DO FOR CONTINUE BREAK RETURN

%start translation_unit
%%

//---A_expList
argument_expression_list
        : assignment_expression 
        | argument_expression_list ',' assignment_expression 
        ;

//---A_exp
primary_expression
        : CONSTANT {
		    switch($1->kind){
		    case L_char: printf("%c", $1->u.c); break;
		    case L_num: printf("%d", $1->u.num); break;
		    case L_real: printf("%lf", $1->u.real); break;
		    }
        }
        | STRING_LITERAL 
        | '(' expression ')'
        ;

//---A_exp
postfix_expression
        : primary_expression 
        | IDENTIFIER '(' ')' 
        | IDENTIFIER '(' argument_expression_list ')' 
        ;

//---A_var
postfix_var
        : IDENTIFIER 
        | postfix_var '[' expression ']' 
        | postfix_var '.' IDENTIFIER 
        | postfix_var PTR_OP IDENTIFIER 
        | '(' unary_var ')'
        ;

unary_var
		: postfix_var
		| '*' unary_var
		;

//---A_exp
unary_expression
        : unary_var
        | postfix_expression 
        | '&' unary_expression 
        | '~' unary_expression 
        | '!' unary_expression  
        ;

//---A_exp
cast_expression
        : unary_expression 
        ;

//---A_exp
multiplicative_expression
        : cast_expression 
        | multiplicative_expression '*' cast_expression  
        | multiplicative_expression '/' cast_expression  
        | multiplicative_expression '%' cast_expression  
        ;

//---A_exp
additive_expression
        : multiplicative_expression 
        | additive_expression '+' multiplicative_expression  
        | additive_expression '-' multiplicative_expression  
        ;

//---A_exp
shift_expression
        : additive_expression 
        | shift_expression LEFT_OP additive_expression  
        | shift_expression RIGHT_OP additive_expression  
        ;

//---A_exp
relational_expression
        : shift_expression 
        | relational_expression '<' shift_expression   
        | relational_expression '>' shift_expression   
        | relational_expression LE_OP shift_expression   
        | relational_expression GE_OP shift_expression   
        ;

//---A_exp
equality_expression
        : relational_expression 
        | equality_expression EQ_OP relational_expression 
        | equality_expression NE_OP relational_expression 
        ;

//---A_exp
and_expression
        : equality_expression 
        | and_expression '&' equality_expression 
        ;

//---A_exp
exclusive_or_expression
        : and_expression 
        | exclusive_or_expression '^' and_expression 
        ;

//---A_exp
inclusive_or_expression
        : exclusive_or_expression 
        | inclusive_or_expression '|' exclusive_or_expression 
        ;

//---A_exp
logical_and_expression
        : inclusive_or_expression 
        | logical_and_expression AND_OP inclusive_or_expression 
        ;

//---A_exp
logical_or_expression
        : logical_and_expression 
        | logical_or_expression OR_OP logical_and_expression 
        ;

//---A_exp
conditional_expression
        : logical_or_expression 
        | logical_or_expression '?' expression ':' conditional_expression 
        ;

//---A_expList
expression
        : assignment_expression 
        | expression ',' assignment_expression 
        ;
        
//---A_exp
assignment_expression
        : conditional_expression 
        | unary_var '=' assignment_expression 
        ;
        
//---A_exp
initializer
        : assignment_expression 
        ;

//---A_efield
init_declarator
        : IDENTIFIER 
        | IDENTIFIER '=' initializer 
        ;

//---A_efieldList
init_declarator_list
        : init_declarator 
        | init_declarator_list ',' init_declarator 
        ;

//---A_exp
constant_expression
        : conditional_expression 
        ;

//---A_decList(begin)
translation_unit
        : external_declaration 
        | translation_unit external_declaration 
        ;
        
//---A_dec
external_declaration
        : function_definition
        | declaration
        | STRUCT IDENTIFIER '{' struct_declaration_list '}'
        ;

//---A_dec
declaration
        : declaration_specifiers init_declarator_list ';' 
        ;

//---A_dec
function_definition
        : declaration_specifiers IDENTIFIER '(' parameter_type_list ')' compound_statement 
        | declaration_specifiers IDENTIFIER '(' ')' compound_statement 
        ;

//---A_ty
declaration_specifiers
        : type_specifier 
        | STRUCT IDENTIFIER 
        | declaration_specifiers '*' 
        | declaration_specifiers '[' constant_expression ']' 
        ;

//---A_ty
type_specifier
        : VOID 
        | CHAR 
        | INT 
        | FLOAT 
        ;

//---A_fieldList
struct_declaration_list
        : struct_declaration 
        | struct_declaration_list struct_declaration 
        ;

//---A_fieldList
struct_declaration
        : declaration_specifiers struct_declarator_list ';'
        ;

//---A_fieldList 
struct_declarator_list
        : IDENTIFIER 
        | struct_declarator_list ',' IDENTIFIER 
        ;
        
//---A_exp
compound_statement
        : '{' '}'
        | '{' statement_list '}'
        | '{' declaration_list '}'
        | '{' declaration_list statement_list '}'
        ;

//---A_decList
declaration_list
        : declaration 
        | declaration_list declaration 
        ;

//---A_expList
statement_list
        : statement 
        | statement_list statement 
        ;

//---A_exp
statement
        : labeled_statement 
        | compound_statement 
        | expression_statement 
        | selection_statement 
        | iteration_statement 
        | jump_statement 
        ;

//---A_exp
labeled_statement
        : CASE constant_expression ':' statement 
        | DEFAULT ':' statement 
        ;

//---A_exp
expression_statement
        : ';' 
        | expression ';' 
        ;

//---A_exp
selection_statement
        : IF '(' expression ')' statement 
        | IF '(' expression ')' statement ELSE statement 
        | SWITCH '(' expression ')' statement 
        ;

//---A_exp
iteration_statement
        : WHILE '(' expression ')' statement 
        | DO statement WHILE '(' expression ')' ';' 
        | FOR '(' expression_statement expression_statement ')' statement 
        | FOR '(' expression_statement expression_statement expression ')' statement 
        ;

//---A_exp
jump_statement
        : CONTINUE ';' 
        | BREAK ';' 
        | RETURN ';' 
        | RETURN expression ';' 
        ;

//---A_fieldList
parameter_type_list
        : parameter_list 
        ;

//---A_fieldList
parameter_list
        : parameter_declaration 
        | parameter_list ',' parameter_declaration 
        ;

//---A_ty
parameter_declaration
        : declaration_specifiers IDENTIFIER 
        | declaration_specifiers 
        ;

%%

int main(){
    error_reset();
    return yyparse();
}

int yyerror(string message){
    puts("yyerror");
    parse_error(-1, message);
    return 1;
}