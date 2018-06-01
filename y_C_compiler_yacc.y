%{

#include "util.h"
#include "lex.yy.c"

extern int pos;

int yyerror(char* msg);

A_ty specifiers_type;

%}

%union {int num; char* id; A_exp exp; A_expList expList; A_var var; A_efield efield; A_efieldList efieldList; 
        L_constant constant; A_dec dec; A_decList decList; A_ty ty; A_fieldList fieldList;}

%token<id> IDENTIFIER STRING_LITERAL
%token<constant> CONSTANT

%token PTR_OP LEFT_OP RIGHT_OP LE_OP GE_OP EQ_OP NE_OP
%token AND_OP OR_OP
%token CHAR INT FLOAT VOID
%token STRUCT
%token CASE DEFAULT IF ELSE SWITCH WHILE DO FOR CONTINUE BREAK RETURN

%type<expList> argument_expression_list expression statement_list
%type<exp> primary_expression postfix_expression unary_expression cast_expression multiplicative_expression additive_expression shift_expression relational_expression equality_expression and_expression exclusive_or_expression inclusive_or_expression logical_and_expression logical_or_expression conditional_expression assignment_expression initializer constant_expression compound_statement statement labeled_statement expression_statement selection_statement iteration_statement jump_statement 
%type<var> postfix_var unary_var
%type<efield> init_declarator
%type<efieldList> init_declarator_list
%type<fieldList> struct_declaration_list struct_declaration struct_declarator_list parameter_type_list parameter_list
%type<A_ty> declaration_specifiers type_specifier parameter_declaration
%type<A_dec> external_declaration declaration function_definition 
%type<A_decList> translation_unit declaration_list

%start translation_unit

%%

//---A_expList
argument_expression_list
        : assignment_expression {$$ = A_ExpList($1, null);}
        | argument_expression_list ',' assignment_expression {$$ = A_ExpList($2, $1);}
        ;

//---A_exp
primary_expression
        : CONSTANT {
                switch($1->kind){
                    case L_char: $$ = A_CharExp(pos, $1->u.c); break;
                    case L_num: $$ = A_IntExp(pos, $1->u.num); break;
                    case L_real: $$ = A_FloatExp(pos, $1->u.real); break;
                }
            }
        | STRING_LITERAL {$$ = A_StringExp(pos, $1);}
        | '(' expression ')' {$$ = A_SeqExp(pos, $2);}
        ;

//---A_exp
postfix_expression
        : primary_expression {$$ = $1;}
        | IDENTIFIER '(' ')' {$$ = A_CallExp(pos, S_Symbol($1), null);}
        | IDENTIFIER '(' argument_expression_list ')' {$$ = A_CallExp(pos, S_Symbol($1), $3);}
        ;

//---A_var
postfix_var
        : IDENTIFIER {$$ = A_SimpleVar(pos, S_Symbol($1));}
        | postfix_var '[' expression ']' {$$ = A_SubscriptVar(pos, $1, A_SeqExp(pos, $3);}
        | postfix_var '.' IDENTIFIER {$$ = A_FieldVar(pos, $1, S_Symbol($3));}
        | postfix_var PTR_OP IDENTIFIER {$$ = A_FieldVar(pos, A_SubscriptVar(pos, $1, A_IntExp(pos, 0)), S_Symbol($3)); }
        | '(' unary_var ')' {$$ = $2;}
        ;

//---A_var
unary_var
        : postfix_var {$$=$1;}
        | '*' unary_var {$$=A_SubscriptVar(pos,$2,A_IntExp(pos, 0));}
        ;

//---A_exp
unary_expression
        : unary_var {$$ = A_VarExp(pos, $1);}
        | postfix_expression {$$ = $1;}
        | '&' unary_expression {$$ = A_UnaryExp(pos, A_adrOp, $2);}
        | '~' unary_expression {$$ = A_UnaryExp(pos, A_bnotOp, $2);}
        | '!' unary_expression {$$ = A_UnaryExp(pos, A_notOp, $2);}
        ;

//---A_exp
cast_expression
        : unary_expression {$$ = $1;}
        ;

//---A_exp
multiplicative_expression
        : cast_expression {$$ = $1;}
        | multiplicative_expression '*' cast_expression  {$$ = A_OpExp(pos, A_timesOp, $1, $3);}
        | multiplicative_expression '/' cast_expression  {$$ = A_OpExp(pos, A_divideOp, $1, $3);}
        | multiplicative_expression '%' cast_expression  {$$ = A_OpExp(pos, A_modOp, $1, $3);}
        ;

//---A_exp
additive_expression
        : multiplicative_expression {$$ = $1;}
        | additive_expression '+' multiplicative_expression  {$$ = A_OpExp(pos, A_plusOp, $1, $3);}
        | additive_expression '-' multiplicative_expression  {$$ = A_OpExp(pos, A_minusOp, $1, $3);}
        ;

//---A_exp
shift_expression
        : additive_expression {$$ = $1;}
        | shift_expression LEFT_OP additive_expression  {$$ = A_OpExp(pos, A_leftOp, $1, $3);}
        | shift_expression RIGHT_OP additive_expression  {$$ = A_OpExp(pos, A_rightOp, $1, $3);}
        ;

//---A_exp
relational_expression
        : shift_expression {$$ = $1;}
        | relational_expression '<' shift_expression   {$$ = A_OpExp(pos, A_ltOp, $1, $3);}
        | relational_expression '>' shift_expression   {$$ = A_OpExp(pos, A_gtOp, $1, $3);}
        | relational_expression LE_OP shift_expression   {$$ = A_OpExp(pos, A_leOp, $1, $3);}
        | relational_expression GE_OP shift_expression   {$$ = A_OpExp(pos, A_geOp, $1, $3);}
        ;

//---A_exp
equality_expression
        : relational_expression {$$ = $1;}
        | equality_expression EQ_OP relational_expression {$$ = A_OpExp(pos, A_eqOp, $1, $3);}
        | equality_expression NE_OP relational_expression {$$ = A_OpExp(pos, A_neqOp, $1, $3);}
        ;

//---A_exp
and_expression
        : equality_expression {$$ = $1;}
        | and_expression '&' equality_expression {$$ = A_OpExp(pos, A_andOp, $1, $3);}
        ;

//---A_exp
exclusive_or_expression
        : and_expression {$$ = $1;}
        | exclusive_or_expression '^' and_expression {$$ = A_OpExp(pos, A_eorOp, $1, $3);}
        ;

//---A_exp
inclusive_or_expression
        : exclusive_or_expression {$$ = $1;}
        | inclusive_or_expression '|' exclusive_or_expression {$$ = A_OpExp(pos, A_orOp, $1, $3);}
        ;

//---A_exp
logical_and_expression
        : inclusive_or_expression {$$ = $1;}
        | logical_and_expression AND_OP inclusive_or_expression {$$ = A_OpExp(pos, A_landOp, $1, $3);}
        ;

//---A_exp
logical_or_expression
        : logical_and_expression {$$ = $1;}
        | logical_or_expression OR_OP logical_and_expression {$$ = A_OpExp(pos, A_lorOp, $1, $3);}
        ;

//---A_exp
conditional_expression
        : logical_or_expression {$$ = $1;}
        | logical_or_expression '?' expression ':' conditional_expression {$$ = A_ConExp(pos, $1, A_SeqExp(pos,$3), $5);}
        ;

//---A_expList
expression
        : assignment_expression {$$ = A_ExpList($1, null);}
        | expression ',' assignment_expression {$$ = A_ExpList($3, $1);}
        ;
        
//---A_exp
assignment_expression
        : conditional_expression {$$ = $1;}
        | unary_var '=' assignment_expression {$$ = A_AssignExp(pos, $1, $3)}
        ;
        
//---A_exp
initializer
        : assignment_expression {$$=$1;}
        ;

//---A_efield
init_declarator
        : IDENTIFIER {$$=A_Efield(S_Symbol($1),null);}
        | IDENTIFIER '=' initializer {$$=A_Efield(S_Symbol($1),$3);}
        ;

//---A_efieldList
init_declarator_list
        : init_declarator {$$=A_EfieldList($1,null);}
        | init_declarator_list ',' init_declarator {$$=A_EfieldList($2,$1);}
        ;

//---A_exp
constant_expression
        : conditional_expression {$$=$1;}
        ;

//---A_decList(begin)
translation_unit
        : external_declaration {$$=A_DecList($1, null);}
        | translation_unit external_declaration {$$=A_DecList($2, $1);}
        ;
        
//---A_dec
external_declaration
        : function_definition {$$=$1;}
        | declaration {$$=$1;}
        | STRUCT IDENTIFIER '{' struct_declaration_list '}' {$$=A_StructDec(pos,A_StructTy(S_Symbol($2)),$4);}
        ;

//---A_dec
declaration
        : declaration_specifiers init_declarator_list ';' {$$=A_VarDec(pos,$2,$1));}
        ;

//---A_dec
function_definition
        : declaration_specifiers IDENTIFIER '(' parameter_type_list ')' compound_statement {$$=A_FunctionDec(pos,S_Symbol($2),$4,$1,$6);}
        | declaration_specifiers IDENTIFIER '(' ')' compound_statement {$$=A_FunctionDec(pos,S_Symbol($2),null,$1,$5);}
        ;

//---A_ty
declaration_specifiers
        : type_specifier {$$=$1;}
        | STRUCT IDENTIFIER {$$=A_StructTy(S_Symbol($2));}
        | declaration_specifiers '*' {$$=A_ArrayTy($1,0);}
        | declaration_specifiers '[' constant_expression ']' {$$=A_ArrayTy($1,$3);}
        ;

//---A_ty
type_specifier
        : VOID {$$=A_NameTy(S_Symbol("void"));}
        | CHAR {$$=A_NameTy(S_Symbol("char"));}
        | INT {$$=A_NameTy(S_Symbol("int"));}
        | FLOAT {$$=A_NameTy(S_Symbol("float"));}
        ;

//---A_fieldList
struct_declaration_list
        : struct_declaration {$$=$1;}
        | struct_declaration_list struct_declaration {$$=A_MergeFieldList($2,$1);}
        ;

//---A_fieldList
struct_declaration
        : declaration_specifiers {specifiers_type=$1;} struct_declarator_list ';' {$$=$3;}
        ;

//---A_fieldList 
struct_declarator_list
        : IDENTIFIER {$$=A_FieldList(A_Field(pos,S_Symbol($1),specifiers_type),null);}
        | struct_declarator_list ',' IDENTIFIER {$$=A_FieldList(A_Field(pos,S_Symbol($3),specifiers_type),$1);}
        ;
        
//---A_exp
compound_statement
        : '{' '}' {$$=A_NilExp(pos);}
        | '{' statement_list '}' {$$=A_SeqExp(pos,$2);} 
        | '{' declaration_list '}' {$$=A_LetExp(pos,$2,null);} 
        | '{' declaration_list statement_list '}' {$$=A_LetExp(pos,$2,$3);} 
        ;

//---A_decList
declaration_list
        : declaration {$$=A_DecList($1,null);}
        | declaration_list declaration {$$=A_DecList($2,$1);}
        ;

//---A_expList
statement_list
        : statement {$$=A_ExpList($1,null);}
        | statement_list statement {$$=A_ExpList($2,$1);}
        ;

//---A_exp
statement
        : labeled_statement {$$=$1;}
        | compound_statement {$$=$1;}
        | expression_statement {$$=$1;}
        | selection_statement {$$=$1;}
        | iteration_statement {$$=$1;}
        | jump_statement {$$=$1;}
        ;

//---A_exp
labeled_statement
        : CASE constant_expression ':' statement {$$=A_CaseExp(pos,$4,A_IntExp($2));}
        | DEFAULT ':' statement {$$=A_CaseExp(pos,$3,null);}
        ;

//---A_exp
expression_statement
        : ';' {$$=A_NilExp(pos);}
        | expression ';' {$$=A_SeqExp(pos,$1);}
        ;

//---A_exp
selection_statement
        : IF '(' expression ')' statement {$$=A_IfExp(pos,A_SeqExp(pos,$3),$5,null);}
        | IF '(' expression ')' statement ELSE statement {$$=A_IfExp(pos,A_SeqExp(pos,$3),$5,$7);}
        | SWITCH '(' expression ')' statement {$$=A_SwitchExp(pos,A_SeqExp(pos,$3),$5);}
        ;

//---A_exp
iteration_statement
        : WHILE '(' expression ')' statement {$$=A_WhileExp(pos,A_SeqExp(pos,$3),$5);}
        | DO statement WHILE '(' expression ')' ';' {$$=A_SeqExp(pos,A_ExpList($2,A_ExpList(A_WhileExp(pos,A_SeqExp(pos,$5),$2),null)));}
        | FOR '(' expression_statement expression_statement ')' statement {$$=A_ForExp(pos,$3,$4,A_NilExp(pos),$6);}
        | FOR '(' expression_statement expression_statement expression ')' statement {$$=A_ForExp(pos,$3,$4,A_SeqExp(pos,$5),$7);}
        ;

//---A_exp
jump_statement
        : CONTINUE ';' {$$=A_ContinueExp(pos);}
        | BREAK ';' {$$=A_BreakExp(pos);}
        | RETURN ';' {$$=A_ReturnExp(pos,null);}
        | RETURN expression ';' {$$=A_ReturnExp(pos,A_SeqExp(pos,$2));}
        ;

//---A_fieldList
parameter_type_list
        : parameter_list {$$=$1;}
        ;

//---A_fieldList
parameter_list
        : parameter_declaration {$$=A_FieldList(A_Field(pos,null,$1),null);}
        | parameter_list ',' parameter_declaration {$$=A_FieldList(A_Field(pos,null,$3),$1);}
        ;

//---A_ty
parameter_declaration
        : declaration_specifiers IDENTIFIER {$$=$1;}
        | declaration_specifiers {$$=$1;}
        ;

%%

int main() {
    return yyparse();
}

int yyerror(char* msg) {
    printf("%s",msg);
}