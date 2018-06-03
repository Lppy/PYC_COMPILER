#include "env.h"

E_enventry E_VarEntry(Ty_ty ty)
{
    E_enventry p = (E_enventry)checked_malloc(sizeof(*p));
    p->kind = E_varEntry;
    p->u.var.ty = ty;
    return p;
}

E_enventry E_FunEntry(Ty_tyList formals, Ty_ty reslut)
{
    E_enventry p = (E_enventry)checked_malloc(sizeof(*p)) ;
    p->kind = E_funEntry;
    p->u.fun.formals = formals;
    p->u.fun.result = reslut;
    return p;
}

bool isTyequTy(Ty_ty s1, Ty_ty s2)
{
    Ty_ty tmp1 = s1;
    Ty_ty tmp2 = s2;
    bool isstruct = (tmp1->kind == Ty_struct || tmp2->kind == Ty_struct);
    bool isary = (tmp1->kind == Ty_array || tmp2->kind == Ty_array);
    bool isnil = (tmp1->kind == Ty_nil || tmp2->kind == Ty_nil);
    if(tmp1->kind != tmp2->kind)
    {
        if (isnil && isary)
        {
            return TRUE;
        }
        return FALSE;
    }
    if(isary)
    {
        return isTyequTy(tmp1->u.array.ty, tmp2->u.array.ty);
    }
    if(isstruct && tmp1 != tmp2)
    {
        return FALSE;
    }
    return TRUE;
}