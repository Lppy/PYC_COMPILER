#include "env.h"

E_enventry E_VarEntry(Tr_access acc, Ty_ty ty)
{
    E_enventry p = (E_enventry)checked_malloc(sizeof(*p));
    p->kind = E_varEntry;
    p->u.var.ty = ty;
    p->u.var.acc = acc;
    return p;
}

E_enventry E_FunEntry(Ty_tyList formals, Ty_ty reslut, Temp_label label, Tr_level level)
{
    E_enventry p = (E_enventry)checked_malloc(sizeof(*p)) ;
    p->kind = E_funEntry;
    p->u.fun.formals = formals;
    p->u.fun.result = reslut;
    p->u.fun.label = label;
    p->u.fun.level = level;
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