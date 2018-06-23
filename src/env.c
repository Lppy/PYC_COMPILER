#include "env.h"

E_enventry E_VarEntry(Tr_access acc, Ty_ty ty)
{
    E_enventry p = (E_enventry)checked_malloc(sizeof(*p));
    p->kind = E_varEntry;
    p->u.var.ty = ty;
    p->u.var.acc = acc;
    return p;
}

E_enventry E_FunEntry(Ty_fieldList formals, Ty_ty reslut, Temp_label label, U_boolList boollist)
{
    E_enventry p = (E_enventry)checked_malloc(sizeof(*p)) ;
    p->kind = E_funEntry;
    p->u.fun.formals = formals;
    p->u.fun.result = reslut;
    p->u.fun.label = label;
    p->u.fun.boollist = boollist;
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
        if (isary && (tmp1->kind ==Ty_int || tmp2->kind == Ty_int) )
            return TRUE;
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

static S_table base_tenv = NULL;
S_table E_base_tenv()
{
    if(!base_tenv){
        base_tenv = S_empty();
    }
    return base_tenv;
}

static S_table base_venv = NULL;
S_table E_base_venv()
{
    if(!base_venv){
        base_venv = S_empty();
    }
    return base_venv;
}
