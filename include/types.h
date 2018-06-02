#ifndef TYPES_H_
#define TYPES_H_

typedef struct Ty_ty_ *Ty_ty;
typedef struct Ty_tyList_ *Ty_tyList;
typedef struct Ty_field_ *Ty_field;
typedef struct Ty_fieldList_ *Ty_fieldList;

struct Ty_ty_ {
	enum {Ty_nil, Ty_int, Ty_float, Ty_char, Ty_string, Ty_array, Ty_struct, Ty_void} kind;
	union {
		struct {S_symbol sym; Ty_fieldList structure;} structt;
		struct {Ty_ty ty; int length;} array;
	} u;
};

struct Ty_tyList_ {Ty_ty head; Ty_tyList tail;};
struct Ty_field_ {S_symbol name; Ty_ty ty;};
struct Ty_fieldList_ {Ty_field head; Ty_fieldList tail;};

bool Ty_IsNum(Ty_ty ty);

Ty_ty Ty_Nil(void);
Ty_ty Ty_Int(void);
Ty_ty Ty_Float(void);
Ty_ty Ty_Char(void);
Ty_ty Ty_String(void);
Ty_ty Ty_Void(void);

Ty_ty Ty_Struct(S_symbol sym, Ty_fieldList fields);
Ty_ty Ty_Array(Ty_ty ty, int len);

Ty_tyList Ty_TyList(Ty_ty head, Ty_tyList tail);
Ty_field Ty_Field(S_symbol name, Ty_ty ty);
Ty_fieldList Ty_FieldList(Ty_field head, Ty_fieldList tail);

void Ty_print(Ty_ty t);
void TyList_print(Ty_tyList list);

#endif