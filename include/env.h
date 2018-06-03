#ifndef ENV_H_
#define ENV_H_

#include "util.h"
#include "symbol.h"
#include "types.h"

typedef struct E_enventry_ *E_enventry;
struct E_enventry_ {
    enum { E_varEntry, E_funEntry } kind;
    union
    {
        struct { Ty_ty ty; } var;
        struct { Ty_tyList formals; Ty_ty result;} fun;
    } u;
};

E_enventry E_VarEntry(Ty_ty ty);
E_enventry E_FunEntry(Ty_tyList formals, Ty_ty reslut);

S_table E_base_tenv();
S_table E_base_venv();

bool isTyequTy(Ty_ty, Ty_ty);

#endif