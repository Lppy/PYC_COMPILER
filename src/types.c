#include "util.h"
#include "symbol.h"
#include "types.h"

static struct Ty_ty_ tynil = {Ty_nil};
Ty_ty Ty_Nil(void) {return &tynil;}

static struct Ty_ty_ tyint = {Ty_int};
Ty_ty Ty_Int(void) {return &tyint;}

static struct Ty_ty_ tyfloat = {Ty_float};
Ty_ty Ty_Float(void) {return &tyfloat;}

static struct Ty_ty_ tychar = {Ty_char};
Ty_ty Ty_Char(void) {return &tychar;}

static struct Ty_ty_ tystring = {Ty_string};
Ty_ty Ty_String(void) {return &tystring;}

static struct Ty_ty_ tyvoid = {Ty_void};
Ty_ty Ty_Void(void) {return &tyvoid;}

Ty_ty Ty_Struct(S_symbol sym, Ty_fieldList fields)
{
	Ty_ty p = checked_malloc(sizeof(*p));
	p->kind=Ty_struct;
	p->u.structt.sym=sym;
	p->u.structt.structure=fields;
	return p;
}

Ty_ty Ty_Array(Ty_ty ty, int len)
{
	Ty_ty p = checked_malloc(sizeof(*p));
	p->kind=Ty_array;
	p->u.array.ty=ty;
	p->u.array.length=len;
	return p;
}

Ty_tyList Ty_TyList(Ty_ty head, Ty_tyList tail)
{
	Ty_tyList p = checked_malloc(sizeof(*p));
	p->head=head;
	p->tail=tail;
	return p;
}

Ty_field Ty_Field(S_symbol name, Ty_ty ty)
{
	Ty_field p = checked_malloc(sizeof(*p));
	p->name=name;
	p->ty=ty;
	return p;
}

Ty_fieldList Ty_FieldList(Ty_field head, Ty_fieldList tail)
{
	Ty_fieldList p = checked_malloc(sizeof(*p));
	p->head=head;
	p->tail=tail;
	return p;
}

bool Ty_IsNum(Ty_ty ty){
	if(ty->kind==Ty_int||ty->kind==Ty_float||ty->kind==Ty_char)
		return TRUE;
	return FALSE;
}

static char str_ty[][12] = {
	"ty_nil", "ty_int", "ty_float", "ty_char", "ty_string", 
	"ty_array", "ty_struct", "ty_void"
};

void Ty_print(Ty_ty t)
{
	if (t == NULL) printf("null");
	else printf("%s", str_ty[t->kind]);
}

void TyList_print(Ty_tyList list)
{
	if (list == NULL) printf("null");
	else {
		printf("TyList( ");
		Ty_print(list->head);
		printf(", ");
		TyList_print(list->tail);
		printf(")");
	}
}

