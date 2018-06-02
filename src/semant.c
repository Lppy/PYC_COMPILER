#include "semant.h"
#include <assert.h>
#include "env.h"
#include "error.h"

struct expty expTy(Tr_exp exp, Ty_ty ty){
    expty e;
    e.exp=exp;e.ty=ty;
    return e;
}

struct expty transExp(S_table venv, S_table tenv, A_exp exp, Tr_level level){
    static bool done  = false;
    if(exp == NULL) assert(0);
    switch(exp->kind){
    case A_varExp:
        return transVar(venv, tenv, exp->u.var, level);
    case A_nilExp:
        return expTy(Tr_nilExp() ,Ty_Nil());
    case A_intExp:
        return expTy(Tr_intExp(exp->u.intt), Ty_Int());
    case A_charExp:
        return expTy(Tr_intExp(exp->u.charr), Ty_Char());
    case A_floatExp:
        return expTy(Tr_intExp(exp->u.floatt), Ty_Float());
    case A_stringExp:
        return expTy(Tr_stringExp(exp->u.stringg), Ty_String());
    case A_callExp:{
        E_enventry tmp =(E_enventry)S_look(venv, exp->u.call.func);
        if(tmp == NULL)
            type_error(exp->pos, "function %s undeclared", exp->u.call.func);
        Ty_tyList tylist  = tmp->u.fun.formals;
        A_expList explist = exp->u.call.args;
        Tr_expList trexplist = NULL;
        while(tylist != NULL && explist != NULL){
            expty exptyp = transExp(venv, tenv, explist->head, level);
            if(exptyp.ty->kind == Ty_nil){
                continue;
            }
            if(!isTyequTy(tylist->head, exptyp.ty))
                type_error(exp->pos, "parameter type unequal");
            trexplist = Tr_ExpList(exptyp.exp, trexplist);
            tylist = tylist->tail;  
            explist = explist->tail;
        }
        if(tylist != NULL || explist != NULL)
            type_error(exp->pos, "unfit parameter count");
        return expTy(Tr_callExp(tmp->u.fun.label, tmp->u.fun.level, level, trexplist), actrulyTy(tmp->u.fun.result)); //no1 
    }
    case A_conExp:{
        //unfinished flag
    }
    case A_opExp:{
        expty tmpleft = transExp(venv, tenv, exp->u.op.left, level);
        expty tmpright = transExp(venv, tenv, exp->u.op.right, level);
        switch(exp->u.op.oper){
            case A_landOp   :
            case A_lorOp    :
            case A_plusOp   : 
            case A_minusOp  : 
            case A_timesOp  : 
            case A_divideOp :{
                if((!Ty_IsNum(tmpleft.ty->kind)) ||(!Ty_IsNum(tmpright.ty->kind)))
                    type_error(exp->pos, "non-numberic type can not be operated");
                return expTy(Tr_binopExp(tmpleft.exp, tmpright.exp, exp->u.op.oper), Ty_Int()); 
            }
            case A_leftOp   :
            case A_rightOp  :
            case A_andOp    :
            case A_orOp     :
            case A_eorOp    :
            case A_modOp    :{
                if((tmpleft.ty->kind!=Ty_int) ||(tmpright.ty->kind!=Ty_int))
                    type_error(exp->pos, "non-int type can not be operated");
                return expTy(Tr_binopExp(tmpleft.exp, tmpright.exp, exp->u.op.oper), Ty_Int()); 
            }
            case A_ltOp     : 
            case A_leOp     : 
            case A_gtOp     : 
            case A_geOp     : 
            {
                if((!Ty_IsNum(tmpleft.ty->kind)) ||(!Ty_IsNum(tmpright.ty->kind)))
                    type_error(exp->pos, "non-numberic type can not be compared");
                return expTy(Tr_relopExp(tmpleft.exp, tmpright.exp, exp->u.op.oper), Ty_Int());
            }
            case A_eqOp     :
            case A_neqOp    :{
                if(Ty_IsNum(tmpleft.ty->kind) && Ty_IsNum(tmpright.ty->kind))
                    return expTy(Tr_relopExp(tmpleft.exp, tmpright.exp, exp->u.op.oper), Ty_Int());
                if(tmpleft.ty->kind == tmpright.ty->kind){
                    if(tmpleft.ty->kind == Ty_struct || tmpleft.ty->kind == Ty_array)
                    if(tmpleft.ty == tmpright.ty)
                        return expTy(Tr_relopExp(tmpleft.exp, tmpright.exp, exp->u.op.oper), Ty_Int());
                type_error(exp->pos, "not comparable");
            }
         }
     }
    type_error(exp->pos, "unknown operater");
    }
    case A_unaryExp:{
        //unfinished flag
    }
    case A_seqExp:{
        A_expList explist = exp->u.seq;
        Tr_expList trexplist = NULL;
        if(explist){
            while(explist->tail){
                trexplist = Tr_ExpList(transExp(venv, tenv, explist->head, level).exp, trexplist);
                explist = explist->tail;
            } 
        } else{
            return expTy(Tr_seqExp(trexplist), Ty_Void());
        }
        trexplist = Tr_ExpList(transExp(venv, tenv, explist->head, level).exp, trexplist);
        Tr_FreeExplist(trexplist); //只释放链表结构 不是放内容(hand)
        return expTy(Tr_seqExp(trexplist), tmp.ty);
    }
    case A_assignExp:{
        expty tmpV = transVar(venv, tenv, exp->u.assign.var, level);
        expty tmpE = transExp(venv, tenv, exp->u.assign.exp, level);
        if((!Ty_IsNum(tmpV.ty->kind)) ||(!Ty_IsNum(tmpE.ty->kind)))
            if(tmpV.ty->kind != tmpE.ty->kind)
                type_error(exp->pos, "only of same type or numbers are assignable");
        return expTy(Tr_assignExp(tmpV.exp, tmpE.exp), Ty_Void());
    }
    case A_ifExp:{
        expty tmptest = transExp(venv, tenv, exp->u.iff.test, level);
        if(!Ty_IsNum(tmptest.ty->kind))
            type_error(exp->pos, "the type is not interpretable");
        expty tmpthen = transExp(venv, tenv, exp->u.iff.then, level);
        if(exp->u.iff.elsee != NULL){
            expty tmpelse = transExp(venv, tenv, exp->u.iff.elsee, level);
            if(tmpthen.ty != tmpelse.ty)
                type_error(exp->pos, "else error");
            return expTy(Tr_ifExp(tmptest.exp, tmpthen.exp, tmpelse.exp), tmpelse.ty);
        }
        if(tmpthen.ty->kind != Ty_void)
            type_error(exp->pos, "if error");
        return expTy(Tr_ifExp(tmptest.exp, tmpthen.exp, NULL), Ty_Void());
    }
    case A_whileExp:{
        expty test = transExp(venv, tenv, exp->u.whilee.test, level);
        if(!Ty_IsNum(test.ty->kind))
            type_error(exp->pos, "the type is not interpretable");
        expty body = transExp(venv, tenv, exp->u.whilee.body, level);
        if(done)
            done = false;
        return expTy(Tr_whileExp(test.exp, body.exp, false), Ty_Void());
    }
    case A_forExp:{
        // S_beginScope(venv);
        expty tmpe1 = transExp(venv, tenv, exp->u.forr.e1, level);
        expty tmpe2 = transExp(venv, tenv, exp->u.forr.e2, level);
        expty tmpe3 = transExp(venv, tenv, exp->u.forr.e3, level);
        expty tmpbody = transExp(venv, tenv, exp->u.forr.body, level);
        // Tr_access acc;
        // acc = Tr_allocLocal(level, exp->u.forr.e2);
        // S_enter(venv, exp->u.forr.var, E_VarEntry(acc ,Ty_Int()));
        // if(tmplo.ty->kind != Ty_ty_::Ty_int || tmphi.ty->kind != Ty_ty_::Ty_int)
        //     assert(0);
        if(!Ty_IsNum(tmpe2.ty->kind))
            type_error(exp->pos, "the type is not interpretable");
        // S_endScope(venv);
        return expTy(Tr_forExp(tmpe1.exp, tmpe2.exp, tmpe2.exp, tmpbody.exp), Ty_Void());
    }
    case A_breakExp:{
        done = true;
        return expTy(Tr_breakExp(), Ty_Void());
    }
    case A_continueExp:{
        //unfinished flag
    }
    case A_letExp:{
        S_beginScope(venv);
        S_beginScope(tenv);
        A_decList declist = exp->u.let.decs;
        Tr_expList trexplist = NULL;
        while(declist){
            trexplist = Tr_ExpList(transDec(venv, tenv, declist->head, level), trexplist);
            declist = declist->tail;
        }
        expty tmp;
        if(exp->u.let.body){
            tmp = transExp(venv, tenv, exp->u.let.body, level);
            tmp = expTy(Tr_letExp(trexplist, tmp.exp), tmp.ty);
        }
        else
            tmp = expTy(Tr_letExp(trexplist, NULL), Ty_Void());        }
        Tr_FreeExplist（trexplist);
        S_endScope(venv);
        S_endScope(tenv);
        return tmp;
    }
case A_arrayExp :
{
    Ty_ty ty =(Ty_ty)S_look(tenv, exp->u.array.typ);
    ty = actrulyTy(ty);
    if(ty == NULL || ty->kind != Ty_ty_::Ty_array)
    {
        assert(0);
    }
    expty tynum = transExp(venv, tenv, exp->u.array.size, level);
    if(tynum.ty->kind != Ty_ty_::Ty_int)
    {
        assert(0);
    }
    expty tyinit = transExp(venv, tenv, exp->u.array.init, level);
    if(tyinit.ty != ty->u.array)
    {
        assert(0);
    }
    return expTy(Tr_arryExp(tynum.exp,  tyinit.exp), ty);
}    
}
assert(0);
}

expty transVar(S_table venv, S_table tenv, A_var var, Tr_level level)
{
    switch(var->kind)
    {
        case A_simpleVar :
        {
            E_enventry tmp =(E_enventry) S_look(venv, var->u.simple);

            if(tmp != NULL && tmp->kind == E_enventry_::E_varEntry)
            {
                return expTy(Tr_simpleVar(tmp->u.var.access, level), actrulyTy(tmp->u.var.ty));
            }
            assert(0);
        }
        case A_fieldVar :
        {
            expty tmpty = transVar(venv, tenv, var->u.field.var, level);
            if(tmpty.ty->kind != Ty_ty_::Ty_record)
            {
                assert(0);
            }
            Ty_fieldList fieldList = tmpty.ty->u.record;
            int num = 0; 
            while(fieldList)
            {
                if(fieldList->head->name == var->u.field.sym )
                {
                    return expTy(Tr_fieldVar(tmpty.exp ,num) , actrulyTy(fieldList->head->ty));
                }
                num ++;
                fieldList = fieldList->tail;
            }
            assert(0);
        }
        case A_subscriptVar :
        {
            expty tmp = transVar(venv, tenv, var->u.subscript.var, level);
            if(tmp.ty->kind != Ty_ty_::Ty_array)
            {
                assert(0);
            }
            expty tmpexp  = transExp(venv, tenv, var->u.subscript.exp, level);
            if(tmpexp.ty->kind != Ty_ty_::Ty_int)
            {
                assert(0);
            }
            return expTy(Tr_subscriptVar(tmp.exp ,tmpexp.exp), tmp.ty);
        }    
    }
    assert(0);
}

Tr_exp transDec(S_table venv, S_table tenv, A_dec dec, Tr_level level)
{
    switch(dec->kind)
    {
        case A_functionDec :
        {
            A_fundecList tmpfun = dec->u.function;
            Tr_level newlevel;
            Tr_access acce;
            U_boolList boollist = NULL;
            Ty_ty reslut = NULL;
            while(tmpfun)
            {
                A_fieldList tmpfeldList = tmpfun->head->params;
                Ty_tyList tylist = NULL;
                while(tmpfeldList)
                {
                    Ty_ty ty =(Ty_ty)S_look(tenv,tmpfeldList->head->typ);
                    if(ty == NULL)
                    {
                        assert(0);
                    }
                    tylist = Ty_TyList(ty, tylist);
                    tmpfeldList = tmpfeldList->tail;
                    boollist = U_BoolList(true, boollist);
                }
                if(innerIdentifiers(tmpfun->head->name))
                {
                    assert(0);
                }
//¿‡À?”⁄…?√˜“ª∏ˆ∫Ø ? ªπ√ª”–∂®“ÂÀ¸
                newlevel = Tr_newLevel(level ,Temp_newlabel() ,boollist); 
                reslut =(Ty_ty)S_look(tenv ,tmpfun->head->result);
                if(reslut == NULL)
                {
                    reslut = Ty_Void();
                }
                S_enter(venv, tmpfun->head->name, E_FunEntry(newlevel, newlevel->frame->name, tylist, reslut));
                tmpfun = tmpfun->tail;
U_ClearBoolList(boollist); // “ÚŒ™÷ª «”√¡Àbool ≤¢√ª”–±£¥ÊÀ¸ À?“‘ªπ“™ Õ∑≈“ªœ¬
boollist = NULL;
}
tmpfun = dec->u.function;
E_enventry funEntry;
Tr_accesslist tr_acceselist, tmpacclist;
Tr_expList trexplist = NULL;
while(tmpfun)
{
    S_beginScope(venv);
    funEntry =(E_enventry) S_look(venv, tmpfun->head->name);
    tmpacclist = tr_acceselist = Tr_formals(funEntry->u.fun.level);
A_fieldList tmpfeldList = tmpfun->head->params; // ’‚¿Ô“™øº¬«µΩparamsµƒ…˙≥…∑Ω Ω »Áπ?∫Õtr_accesslist≤ª“ª÷¬“™∏ƒ“ªœ¬
while(tmpfeldList)
{
    Ty_ty ty =(Ty_ty)S_look(tenv,tmpfeldList->head->typ);
    if(innerIdentifiers(tmpfeldList->head->name))
    {
        assert(0);
    }
    S_enter(venv ,tmpfeldList->head->name, E_VarEntry(tmpacclist->head,ty));
    tmpfeldList = tmpfeldList->tail;
    tmpacclist =  tmpacclist->tial;
}
expty  tmp = transExp(venv, tenv, tmpfun->head->body, newlevel);
trexplist = Tr_ExpList(tmp.exp, trexplist);
if(tmp.ty != actrulyTy(funEntry->u.fun.result))
{
    assert(0);
}
Tr_ClearAcces(tr_acceselist); //÷ª π”√¡Àhead tail≤ø∑÷«Â¿Ì∏…æª
S_endScope(venv);
tmpfun = tmpfun->tail;
}
return Tr_funDec(trexplist);
}
case A_typeDec :
{
    A_nametyList namelist = dec->u.type;
    while(namelist)
    {
        if(innerIdentifiers(namelist->head->name))
        {
            assert(0);
        }
// ¥¶¿Ìµ›πÈ ¿‡À?”⁄ …?√˜“ª∏ˆ¿‡–Õ µ´ «ªπ√ª”–∂®“ÂÀ¸
        S_enter(tenv, namelist->head->name ,Ty_Name(namelist->head->name, NULL));
        namelist = namelist->tail;
    }
    namelist = dec->u.type;
    while(namelist)
    {
// ¥¶¿Ìµ›πÈ
        Ty_ty tmp1 = transTy(tenv, namelist->head->ty);
        Ty_ty tmp2 =(Ty_ty)S_look(tenv, namelist->head->name);

        if(  tmp1->kind == Ty_ty_::Ty_int 
            || tmp1->kind == Ty_ty_::Ty_string 
            || tmp1->kind == Ty_ty_::Ty_nil
            || tmp1->kind == Ty_ty_::Ty_void)
        {
//ƒ⁄÷√¿‡–Õ π”√µƒ «Õ¨“ª«¯”Úƒ⁄¥Ê Œ™¡À±£≥÷ø…“‘æ≠––÷∏’Îµƒ±»ΩœæÕ÷±Ω”»∑∂® «∑ÒŒ™Õ¨“ª∏ˆ¿‡–Õ ƒ«√¥æÕ“™÷±Ω”∞—∞Û∂®∏¯ªª¡À
            tmp2 =(Ty_ty)S_changeBind(tenv, namelist->head->name, tmp1);
            tmp2 =(Ty_ty)freeTy(tmp2);
        }
        else
        {
//»Áπ?≤ª «’‚Àƒ÷÷ƒ⁄÷√¿‡–Õ “™œ˙ªŸ 
            tyCpy(tmp2, tmp1);
            tmp1 =(Ty_ty)freeTy(tmp1);
        }
        namelist = namelist->tail;
    }
    namelist = dec->u.type;
    while(namelist)
{  // ¥¶¿Ì —≠ª∑µ›πÈ  ¿?»Á  type a = b  type b = a 
    Ty_ty tmp =(Ty_ty)S_look(tenv, namelist->head->name);
    if(!actrulyTy(tmp))
    {
        assert(0);
    }
    namelist = namelist->tail;
}
return Tr_typeDec();
}
case A_varDec :
{
    if(dec->u.var.init == NULL)
    {
        assert(0);
    }
    expty tmp = transExp(venv, tenv, dec->u.var.init, level);
    if((dec->u.var.typ != NULL))
    {
        if(actrulyTy((Ty_ty)S_look(tenv ,dec->u.var.typ)) != tmp.ty)
        {
            assert(0);
        }
    }
    if(innerIdentifiers(dec->u.var.var))
    {
        assert(0);
    }
    Tr_access acc = Tr_allocLocal(level, dec->u.var.escape);
    S_enter(venv, dec->u.var.var ,E_VarEntry(acc ,tmp.ty));
    return Tr_varDec(acc, tmp.exp);
}    
}
assert(0);
}

Ty_ty transTy(S_table tenv, A_ty ty)
{
    switch(ty->kind)
    {
        case A_nameTy :
        {
            if(S_Symbol("int") == ty->u.name)
            {
                return Ty_Int();
            }
            if(S_Symbol("string") == ty->u.name)
            {
                return Ty_String();
            }
            Ty_ty tmp =(Ty_ty)S_look(tenv, ty->u.name);
            if(tmp == NULL)
            {
                assert(0);
            }
            return Ty_Name(ty->u.name, tmp);
        }
        case A_recordTy :
        {
            A_fieldList tmpfeldList = ty->u.record;
            Ty_fieldList tyfdlist = NULL; 
            while(tmpfeldList)
            {
                Ty_ty  tmp =(Ty_ty)S_look(tenv, tmpfeldList->head->typ);
                if(tmp == NULL)
                {
                    assert(0);
                }
                if(innerIdentifiers(tmpfeldList->head->name))
                {
                    assert(0);
                }
                tyfdlist = Ty_FieldList(Ty_Field(tmpfeldList->head->name, tmp), tyfdlist);
                tmpfeldList = tmpfeldList->tail;
            }
            return Ty_Record(tyfdlist);
        }
        case A_arrayTy :
        {
            Ty_ty tmp  =(Ty_ty)S_look(tenv, ty->u.array);
            if(tmp == NULL)
            {
                assert(0);
            }
            return Ty_Array(tmp);
        }
    }
    assert(0);
}

bool innerIdentifiers(S_symbol sym)
{
    if(sym == S_Symbol("int") || sym == S_Symbol("string"))
    {
        return true;
    }
    return false;
}