#ifndef ENV_H_
#define ENV_H_

#include "util.h"
#include "symbol.h"
#include "types.h"
#include "translate.h"

typedef struct E_enventry_ *E_enventry;
struct E_enventry_ {
    enum { E_varEntry, E_funEntry } kind;
    union
    {
        struct { Ty_ty ty; Tr_access acc; } var;
        struct { Ty_tyList formals; Ty_ty result; Temp_label label; Tr_level level; } fun;
    } u;
};

E_enventry E_VarEntry(Tr_access acc, Ty_ty ty);
E_enventry E_FunEntry(Ty_tyList formals, Ty_ty reslut, Temp_label label, Tr_level level);

S_table E_base_tenv();
S_table E_base_venv();

bool isTyequTy(Ty_ty, Ty_ty);

#endif