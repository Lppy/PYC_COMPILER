#include "util.h"
#include "symbol.h" /* symbol table data structures */
#include "absyn.h"  /* abstract syntax data structures */

A_var A_SimpleVar(A_pos pos, S_symbol sym)
{
	A_var p = checked_malloc(sizeof(*p));
	p->kind=A_simpleVar;
	p->pos=pos;
	p->u.simple=sym;
	return p;
}

A_var A_FieldVar(A_pos pos, A_var var, S_symbol sym)
{
	A_var p = checked_malloc(sizeof(*p));
	p->kind=A_fieldVar;
	p->pos=pos;
	p->u.field.var=var;
	p->u.field.sym=sym;
	return p;
}

A_var A_SubscriptVar(A_pos pos, A_var var, A_exp exp)
{
	A_var p = checked_malloc(sizeof(*p));
	p->kind=A_subscriptVar;
	p->pos=pos;
	p->u.subscript.var=var;
	p->u.subscript.exp=exp;
	return p;
}


A_exp A_VarExp(A_pos pos, A_var var)
{
	A_exp p = checked_malloc(sizeof(*p));
	p->kind=A_varExp;
	p->pos=pos;
	p->u.var=var;
	return p;
}

A_exp A_NilExp(A_pos pos)
{
	A_exp p = checked_malloc(sizeof(*p));
	p->kind=A_nilExp;
	p->pos=pos;
	return p;
}

A_exp A_IntExp(A_pos pos, int i)
{
	A_exp p = checked_malloc(sizeof(*p));
	p->kind=A_intExp;
	p->pos=pos;
	p->u.intt=i;
	return p;
}

A_exp A_CharExp(A_pos pos, char c)
{
	A_exp p = checked_malloc(sizeof(*p));
	p->kind=A_charExp;
	p->pos=pos;
	p->u.charr=c;
	return p;
}

A_exp A_FloatExp(A_pos pos, double d)
{
	A_exp p = checked_malloc(sizeof(*p));
	p->kind=A_floatExp;
	p->pos=pos;
	p->u.floatt=d;
	return p;
}

A_exp A_StringExp(A_pos pos, string s)
{
	A_exp p = checked_malloc(sizeof(*p));
	p->kind=A_stringExp;
	p->pos=pos;
	p->u.stringg=s;
	return p;
}

A_exp A_CallExp(A_pos pos, S_symbol func, A_expList args)
{
	A_exp p = checked_malloc(sizeof(*p));
	p->kind=A_callExp;
	p->pos=pos;
	p->u.call.func=func;
	p->u.call.args=args;
	return p;
}

A_exp A_ConExp(A_pos pos, A_exp left, A_exp mid, A_exp right)
{
	A_exp p = checked_malloc(sizeof(*p));
	p->kind=A_conExp;
	p->pos=pos;
	p->u.con.left=left;
	p->u.con.mid=mid;
	p->u.con.right=right;
	return p;
}

A_exp A_OpExp(A_pos pos, A_oper oper, A_exp left, A_exp right)
{
	A_exp p = checked_malloc(sizeof(*p));
	p->kind=A_opExp;
	p->pos=pos;
	p->u.op.oper=oper;
	p->u.op.left=left;
	p->u.op.right=right;
	return p;
}

A_exp A_UnaryExp(A_pos pos, A_unoper oper, A_exp exp)
{
	A_exp p = checked_malloc(sizeof(*p));
	p->kind=A_unaryExp;
	p->pos=pos;
	p->u.unary.unoper=oper;
	p->u.unary.exp=exp;
	return p;
}

A_exp A_SeqExp(A_pos pos, A_expList seq)
{
	A_exp p = checked_malloc(sizeof(*p));
	p->kind=A_seqExp;
	p->pos=pos;
	p->u.seq=seq;
	return p;
}

A_exp A_AssignExp(A_pos pos, A_var var, A_exp exp)
{
	A_exp p = checked_malloc(sizeof(*p));
	p->kind=A_assignExp;
	p->pos=pos;
	p->u.assign.var=var;
	p->u.assign.exp=exp;
	return p;
}

A_exp A_IfExp(A_pos pos, A_exp test, A_exp then, A_exp elsee)
{
	A_exp p = checked_malloc(sizeof(*p));
	p->kind=A_ifExp;
	p->pos=pos;
	p->u.iff.test=test;
	p->u.iff.then=then;
	p->u.iff.elsee=elsee;
	return p;
}

A_exp A_WhileExp(A_pos pos, A_exp test, A_exp body)
{
	A_exp p = checked_malloc(sizeof(*p));
	p->kind=A_whileExp;
	p->pos=pos;
	p->u.whilee.test=test;
	p->u.whilee.body=body;
	return p;
}

A_exp A_ForExp(A_pos pos, A_exp e1, A_exp e2, A_exp e3, A_exp body)
{
	A_exp p = checked_malloc(sizeof(*p));
	p->kind=A_forExp;
	p->pos=pos;
	p->u.forr.e1=e1;
	p->u.forr.e2=e2;
	p->u.forr.e3=e3;
	p->u.forr.body=body;
	return p;
}

A_exp A_BreakExp(A_pos pos)
{
	A_exp p = checked_malloc(sizeof(*p));
	p->kind=A_breakExp;
	p->pos=pos;
	return p;
}

A_exp A_ContinueExp(A_pos pos)
{
	A_exp p = checked_malloc(sizeof(*p));
	p->kind=A_continueExp;
	p->pos=pos;
	return p;
}

A_exp A_LetExp(A_pos pos, A_decList decs, A_exp body)
{
	A_exp p = checked_malloc(sizeof(*p));
	p->kind=A_letExp;
	p->pos=pos;
	p->u.let.decs=decs;
	p->u.let.body=body;
	return p;
}

A_exp A_CaseExp(A_pos pos, A_exp constant, A_exp body)
{
	A_exp p = checked_malloc(sizeof(*p));
	p->kind=A_caseExp;
	p->pos=pos;
	p->u.casee.constant=constant;
	p->u.casee.body=body;
	return p;
}

A_exp A_SwitchExp(A_pos pos, A_exp test, A_exp body)
{
	A_exp p = checked_malloc(sizeof(*p));
	p->kind=A_switchExp;
	p->pos=pos;
	p->u.switchh.test=test;
	p->u.switchh.body=body;
	return p;
}

A_exp A_ReturnExp(A_pos pos, A_exp res)
{
	A_exp p = checked_malloc(sizeof(*p));
	p->kind=A_returnExp;
	p->pos=pos;
	p->u.returnn.res=res;
	return p;
}

A_dec A_FunctionDec(A_pos pos, S_symbol name, A_fieldList params, A_ty result, A_exp body)
{
	A_dec p = checked_malloc(sizeof(*p));
	p->kind=A_functionDec;
	p->pos=pos;
	p->u.function.name=name;
	p->u.function.params=params;
	p->u.function.result=result;
	p->u.function.body=body;
	return p;
}

A_dec A_VarDec(A_pos pos, A_efieldList varList, A_ty typ)
{
	A_dec p = checked_malloc(sizeof(*p));
	p->kind=A_varDec;
	p->pos=pos;
	p->u.var.varList=varList;
	p->u.var.typ=typ;
	p->u.var.escape=TRUE;
	return p;
}

A_dec A_StructDec(A_pos pos, S_symbol name, A_fieldList structure)
{
	A_dec p = checked_malloc(sizeof(*p));
	p->kind=A_structDec;
	p->pos=pos;
	p->u.structt.name=name;
	p->u.structt.structure=structure;
	return p;
}

A_ty A_NameTy(S_symbol name)
{
	A_ty p = checked_malloc(sizeof(*p));
	p->kind=A_nameTy;
	p->u.name=name;
	return p;
}

A_ty A_StructTy(S_symbol name)
{	
	A_ty p = checked_malloc(sizeof(*p));
	p->kind=A_structTy;
	p->u.name=name;
	return p;
}

A_ty A_ArrayTy(A_ty tar, int length)
{
	A_ty p = checked_malloc(sizeof(*p));
	p->kind=A_arrayTy;
	p->u.array.tar=tar;
	p->u.array.length=length;
	return p;
}

A_field A_Field(A_pos pos, S_symbol name, A_ty typ)
{
	A_field p = checked_malloc(sizeof(*p));
	p->pos=pos;
	p->name=name;
	p->typ=typ;
	p->escape=TRUE;
	return p;
}

A_fieldList A_FieldList(A_field head, A_fieldList tail)
{
	A_fieldList p = checked_malloc(sizeof(*p));
	p->head=head;
	p->tail=tail;
	return p;
}

A_fieldList A_MergeFieldList(A_fieldList head, A_fieldList tail)
{
	A_fieldList oldp = NULL, p = head;
	while(p!=NULL) { oldp = p; p=p->tail; }
	oldp->tail = tail;
	return head;
}

A_expList A_ExpList(A_exp head, A_expList tail)
{
	A_expList p = checked_malloc(sizeof(*p));
	p->head=head;
	p->tail=tail;
	return p;
}

A_decList A_DecList(A_dec head, A_decList tail)
{
	A_decList p = checked_malloc(sizeof(*p));
	p->head=head;
	p->tail=tail;
	return p;
}

A_efield A_Efield(S_symbol name, A_exp exp)
{
	A_efield p = checked_malloc(sizeof(*p));
	p->name=name;
	p->exp=exp;
	return p;
}

A_efieldList A_EfieldList(A_efield head, A_efieldList tail)
{
	A_efieldList p = checked_malloc(sizeof(*p));
	p->head=head;
	p->tail=tail;
	return p;
}

