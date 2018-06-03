#include "semant.h"
#include <assert.h>
#include "env.h"
#include "error.h"

struct expty expTy(Tr_exp exp, Ty_ty ty){
    struct expty e;
    e.exp=exp;e.ty=ty;
    return e;
}

struct expty transExp(S_table venv, S_table tenv, A_exp exp, Tr_level level){
    static bool done  = FALSE;
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
    case A_callExp:
        {
            E_enventry tmp =(E_enventry)S_look(venv, exp->u.call.func);
            if(tmp == NULL)
                type_error(exp->pos, "function %s undeclared", S_name(exp->u.call.func));
            Ty_tyList tylist  = tmp->u.fun.formals;
            A_expList explist = exp->u.call.args;
            Tr_expList trexplist = NULL;
            while(tylist != NULL && explist != NULL){
                struct expty exptyp = transExp(venv, tenv, explist->head, level);
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
            return expTy(Tr_callExp(tmp->u.fun.label, tmp->u.fun.level, level, trexplist), pointedTy(tmp->u.fun.result));
        }
    case A_conExp:
        {
            struct expty tmptest = transExp(venv, tenv, exp->u.con.left, level);
            struct expty tmpleft = transExp(venv, tenv, exp->u.con.mid, level);
            struct expty tmpright = transExp(venv, tenv, exp->u.con.right, level);
            if(!Ty_IsNum(tmptest.ty->kind))
                type_error(exp->pos, "the type is not interpretable");
            if(tmpleft.ty->kind != tmpright.ty->kind)
                type_error(exp->pos, "sry for our stupid brain but now conditional \
                                expression only support candidates of equal type");
            return expTy(Tr_conExp(tmptest.exp, tmpleft.exp, tmpright.exp), tmpleft.ty);
        }
    case A_opExp:
        {
            struct expty tmpleft = transExp(venv, tenv, exp->u.op.left, level);
            struct expty tmpright = transExp(venv, tenv, exp->u.op.right, level);
            switch(exp->u.op.oper)
            {
            case A_landOp: case A_lorOp: case A_plusOp:
            case A_minusOp: case A_timesOp: case A_divideOp:
                {
                    Ty_ty rtype;
                    if((!Ty_IsNum(tmpleft.ty->kind)) ||(!Ty_IsNum(tmpright.ty->kind)))
                        type_error(exp->pos, "non-numberic type can not be operated");
                    if(tmpleft.ty->kind == Ty_float || tmpright.ty->kind == Ty_float)
                        rtype=Ty_Float();
                    else if(tmpleft.ty->kind == Ty_int || tmpright.ty->kind == Ty_int)
                        rtype=TyInt();
                    else
                        rtype=Ty_Char();
                    return expTy(Tr_binopExp(tmpleft.exp, tmpright.exp, exp->u.op.oper), rtype);
                } 
            case A_leftOp: case A_rightOp: case A_andOp:
            case A_orOp: case A_eorOp: case A_modOp:
                {
                    if((tmpleft.ty->kind!=Ty_int) || (tmpright.ty->kind!=Ty_int))
                        type_error(exp->pos, "non-int type can not be operated");
                    return expTy(Tr_binopExp(tmpleft.exp, tmpright.exp, exp->u.op.oper), Ty_Int()); 
                }
            case A_ltOp: case A_leOp: case A_gtOp: case A_geOp: 
                {
                    if((!Ty_IsNum(tmpleft.ty->kind)) || (!Ty_IsNum(tmpright.ty->kind)))
                        type_error(exp->pos, "non-numberic type can not be compared");
                    return expTy(Tr_relopExp(tmpleft.exp, tmpright.exp, exp->u.op.oper), Ty_Int());
                }
            case A_eqOp: case A_neqOp:
                {
                    if(Ty_IsNum(tmpleft.ty->kind) && Ty_IsNum(tmpright.ty->kind))
                        return expTy(Tr_relopExp(tmpleft.exp, tmpright.exp, exp->u.op.oper), Ty_Int());
                    if(tmpleft.ty->kind == tmpright.ty->kind){
                        if(tmpleft.ty->kind == Ty_struct || tmpleft.ty->kind == Ty_array)
                            if(tmpleft.ty == tmpright.ty)
                                return expTy(Tr_relopExp(tmpleft.exp, tmpright.exp, exp->u.op.oper), Ty_Int());
                    }
                    type_error(exp->pos, "not comparable");
                }
            default: 
                type_error(exp->pos, "unknown operater");
            }
        }
    case A_unaryExp:
        {
            tmp = transExp(venv, tenv, exp->u.unary.exp, level);
            switch(exp->u.unary.unoper){
            /*
            case A_ptrOp:
                if(tmp.ty->kind != Ty_array)
                    type_error(exp->pos, "not a pointer type");
                return expT(Tr_unaryopExp(tmp.exp, exp->u.unary.oper), tmp.ty->u.array.ty);
            case A_adrOp://unfinished flag
                return expT(Tr_unaryopExp(tmp.exp, exp->u.unary.oper), Ty_Int());
            */
            case A_notOp:// suppose after not the type is int
                if(!Ty_IsNum(tmp.ty->kind))
                    type_error(exp->pos, "cannot apply not on non-numberic type");
                return expTy(Tr_unaryopExp(tmp.exp, exp->u.unary.unoper), Ty_Int());
            case A_bnotOp:
                if(tmp.ty->kind==Ty_int)
                    return expTy(Tr_unaryopExp(tmp.exp, exp->u.unary.unoper), Ty_Int());
                else if(tmp.ty->kind==Ty_char)
                    return expTy(Tr_unaryopExp(tmp.exp, exp->u.unary.unoper), Ty_Char());
                else
                    type_error(exp->pos, "cannot apply binary not on non-integer type");
            }
            type_error(exp->pos, "unknown unary operater");
        }
    case A_seqExp:
        {
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
    case A_assignExp:
        {
            struct expty tmpV = transVar(venv, tenv, exp->u.assign.var, level);
            struct expty tmpE = transExp(venv, tenv, exp->u.assign.exp, level);
            if((!Ty_IsNum(tmpV.ty->kind)) || (!Ty_IsNum(tmpE.ty->kind)))
                if(tmpV.ty->kind != tmpE.ty->kind)
                    type_error(exp->pos, "only of same type or numbers are assignable");
            return expTy(Tr_assignExp(tmpV.exp, tmpE.exp), Ty_Void());
        }
    case A_ifExp:
        {
            struct expty tmptest = transExp(venv, tenv, exp->u.iff.test, level);
            if(!Ty_IsNum(tmptest.ty->kind))
                type_error(exp->pos, "the type is not interpretable");
            struct expty tmpthen = transExp(venv, tenv, exp->u.iff.then, level);
            if(exp->u.iff.elsee != NULL){
                struct expty tmpelse = transExp(venv, tenv, exp->u.iff.elsee, level);
                if(tmpthen.ty != tmpelse.ty)
                    type_error(exp->pos, "else error");
                return expTy(Tr_ifExp(tmptest.exp, tmpthen.exp, tmpelse.exp), tmpelse.ty);
            }
            if(tmpthen.ty->kind != Ty_void)
                type_error(exp->pos, "if error");
            return expTy(Tr_ifExp(tmptest.exp, tmpthen.exp, NULL), Ty_Void());
        }
    case A_whileExp:
        {
            struct expty test = transExp(venv, tenv, exp->u.whilee.test, level);
            if(!Ty_IsNum(test.ty->kind))
                type_error(exp->pos, "the type is not interpretable");
            struct expty body = transExp(venv, tenv, exp->u.whilee.body, level);
            if(done) done = FALSE;
            return expTy(Tr_whileExp(test.exp, body.exp, FALSE), Ty_Void());
        }
    case A_forExp:
        {
            // S_beginScope(venv);
            struct expty tmpe1 = transExp(venv, tenv, exp->u.forr.e1, level);
            struct expty tmpe2 = transExp(venv, tenv, exp->u.forr.e2, level);
            struct expty tmpe3 = transExp(venv, tenv, exp->u.forr.e3, level);
            struct expty tmpbody = transExp(venv, tenv, exp->u.forr.body, level);
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
    case A_breakExp:
        done = TRUE;
        return expTy(Tr_breakExp(), Ty_Void());
    case A_continueExp:
        return expTy(Tr_continueExp(), Ty_Void());
    case A_letExp:
        {
            struct expty tmp;
            S_beginScope(venv);
            S_beginScope(tenv);
            A_decList declist = exp->u.let.decs;
            Tr_expList trexplist = NULL;
            while(declist){
                trexplist = Tr_ExpList(transDec(venv, tenv, declist->head, level), trexplist);
                declist = declist->tail;
            }
            if(exp->u.let.body){
                tmp = transExp(venv, tenv, exp->u.let.body, level);
                tmp = expTy(Tr_letExp(trexplist, tmp.exp), tmp.ty);
            }
            else
                tmp = expTy(Tr_letExp(trexplist, NULL), Ty_Void());
            Tr_FreeExplist(trexplist);
            S_endScope(venv);
            S_endScope(tenv);
            return tmp;
        }
    case A_switchExp:
        {
            struct expty tmptest = transExp(venv, tenv, exp->u.switchh.test, level);
            A_expList body = exp->u.switchh.bodyList;
            Tr_expList bodylist = NULL;
            if(tmptest.ty.kind != Ty_int || tmptest.ty.kind != Ty_char)
                type_error(exp->pos, "only char or int are allowed to be switched");
            while(body){
                struct expty tmpcon = transExp(venv, tenv, body->head->u.casee.constant, level);
                struct expty tmpbody = transExp(venv, tenv, body->head->u.casee.body, level);
                if(tmpcon.ty.kind != Ty_int && tmpcon.ty.kind != Ty_char)
                    type_error(exp->pos, "only char or int are allowed to be as cases");
                bodylist = Tr_ExpList(Tr_caseExp(tmptest.exp, tmpcon.exp, tmpbody.exp), bodylist);
                body = body->tail;
            }
            return expTy(Tr_switchExp(bodylist), Ty_Void());
        }
    case A_returnExp:{
            struct expty tmp = transExp(venv, tenv, exp->u.returnn, level);
            tmp = expTy(Tr_returnExp(tmp), tmp.ty);
        }
    default: assert(0);
    }
}

struct expty transVar(S_table venv, S_table tenv, A_var var, Tr_level level) {
    switch(var->kind){
    case A_simpleVar:
        {
            E_enventry tmp =(E_enventry)S_look(venv, var->u.simple);
            if(tmp != NULL && tmp->kind == E_varEntry)
                return expTy(Tr_simpleVar(tmp->u.var.access, level), pointedTy(tmp->u.var.ty)); //?????????????
            else if(tmp == NULL)
                type_error(var->pos, "variable not defined");
            else
                type_error(var->pos, "function name cannot be used as variable");
        }
    case A_fieldVar:
        {
            int num = 0;
            struct expty tmp = transVar(venv, tenv, var->u.field.var, level);
            Ty_fieldList fieldList = tmp.ty->u.structt.structure;
            if(tmp.ty->kind != Ty_struct)
                type_error(var->pos, "not a struct type");
            while(fieldList){
                if(fieldList->head->name == var->u.field.sym)
                    return expTy(Tr_fieldVar(tmp.exp, num), pointedTy(fieldList->head->ty));
                num++;
                fieldList = fieldList->tail;
            }
            type_error(var->pos, "structure %s, does not have such a component", S_name(var->u.field.var->u.simple));
        }
    case A_subscriptVar:
        {
            struct expty tmpvar = transVar(venv, tenv, var->u.subscript.var, level);
            struct expty tmpexp = transExp(venv, tenv, var->u.subscript.exp, level);
            if(tmpvar.ty->kind != Ty_array)
                type_error(var->pos, "not a pointer type");
            if(tmpexp.ty->kind != Ty_int || tmpexp.ty->kind != Ty_char)
                type_error(var->pos, "the index is not integer");
            return expTy(Tr_subscriptVar(tmpvar.exp ,tmpexp.exp), tmpvar.ty);
        }
    default: 
        assert(0);
    }
}

Tr_exp transDec(S_table venv, S_table tenv, A_dec dec, Tr_level level){
    switch(dec->kind){
    case A_functionDec:
        {
            S_symbol name = dec->u.function.name;
            A_fieldList para = dec->u.function.params;
            A_ty result = dec->u.function.result;
            A_exp body = dec->u.function.body;

            Tr_level newlevel;
            Tr_access acce;
            U_boolList boollist = NULL;
            Ty_ty res = NULL;
            Ty_tyList tylist = NULL;

            E_enventry funEntry;
            Tr_accesslist tr_acceselist, tmpacclist;
            struct expty tmp;

            while(para){
                Ty_ty ty = transTy(tenv, para->head->typ);
                if(!ty)
                    type_error(dec->pos, "unknown type: %s", S_name(para->head->name));
                tylist = Ty_TyList(ty, tylist);
                para = para->tail;
                boollist = U_BoolList(TRUE, boollist);
            }
            if(innerIdentifiers(name))
                type_error(dec->pos, "cannot use inner type as function name");
            newlevel = Tr_newLevel(level, Temp_newlabel(), boollist);
            // res = (Ty_ty)S_look(tenv, result);
            res = transTy(tenv, result);
            if(!res)
                type_error(dec->pos, "unknown return type");
            funEntry = E_FunEntry(newlevel, newlevel->frame->name, tylist, res);
            S_enter(venv, S_Symbol(name), funEntry);
            U_ClearBoolList(boollist);

            S_beginScope(venv);
            tmpacclist = tr_acceselist = Tr_formals(funEntry->u.fun.level);
            para = dec->u.function.params;
            while(para){
                // Ty_ty ty =(Ty_ty)S_look(tenv, para->head->typ);
                Ty_ty ty = transTy(tenv, para->head->typ);
                S_enter(venv, S_Symbol(name), E_VarEntry(tmpacclist->head, ty));
                para = para->tail;
                tmpacclist = tmpacclist->tail;
            }
            tmp = transExp(venv, tenv, body, newlevel);
            if(isTyequTy(tmp.ty, funEntry->u.fun.result))
                type_error(dec->pos, "return type not matched");
            Tr_ClearAcces(tr_acceselist);
            S_endScope(venv);
            return Tr_funDec(tmp.exp);
        }
    case A_varDec:
        {
            A_efieldList vars = dec->u.var.varList;
            Tr_expList initList = NULL;
            Tr_accesslist accList = NULL;
            if(!vars)assert(0);
            while(vars){
                A_efield var = vars->head;
                Tr_access acc = Tr_allocLocal(level, dec->u.var.escape);
                struct expty tmp = transExp(venv, tenv, var->exp, level);
                Tr_ExpList(tmp.exp, initList);
                Tr_Accesslist(acc, accList);
                if(!isTyequTy(transTy(tenv, dec->u.var.typ), tmp.ty))
                    type_error(dec->pos, "variable type not matched");
                S_enter(venv, var->name, E_VarEntry(acc, tmp.ty));
                vars=vars->tail;
            }
            return Tr_varDec(accList, initList);
        }
    case A_structDec:
        {
            Ty_ty struct_ty; 
            A_fieldList tmp = dec->u.structt.structure;
            if(innerIdentifiers(dec->u.structt.name) || S_look(tenv, dec->u.structt.name))
            {
                assert(0);
            }
            S_enter(tenv, dec->u.structt.name, dec->u.structt.name);
            struct_ty = transTy(tenv, dec->u.structt.name);
            if(tmp){
                Ty_fieldList structure = Ty_FieldList(Ty_Field(tmp->head->name, transTy(tenv, tmp->head->typ)), NULL);
                tmp = tmp->tail;
                while(tmp){
                    structure = Ty_FieldList(Ty_Field(tmp->head->name, transTy(tenv, tmp->head->typ)), structure);
                    tmp = tmp->tail;
                }
                struct_ty->u.structt.structure = structure;
            }
            else
                struct_ty->u.structt.structure = NULL;
            return Tr_StructDec();
        }

    }
    assert(0);
}


Ty_ty transTy(S_table tenv, A_ty ty)
{
    switch(ty->kind)
    {
    case A_nameTy:
        {
            if(S_Symbol("int") == ty->u.name)
            {
                return Ty_Int();
            }
            if(S_Symbol("char") == ty->u.name)
            {
                return Ty_Char();
            }
            if(S_Symbol("float") == ty->u.name)
            {
                return Ty_Float();
            }
            if(S_Symbol("void") == ty->u.name)
            {
                return Ty_Void();
            }
            if(S_Symbol("string") == ty->u.name)
            {
                return Ty_String();
            }
            assert(0);
        }
    case A_structTy:
        {
            Ty_ty tmp;
            if(innerIdentifiers(ty->u.name))
            {
                assert(0);
            }
            tmp = (Ty_ty)S_look(tenv, ty->u.name);
            if(tmp == NULL)
            {
                assert(0);
            }
            return tmp;
        }
    case A_arrayTy:
        {
            Ty_ty tar = transTy(tenv, ty->u.array.tar);
            if(tar == NULL)
            {
                assert(0);
            }
            return Ty_Array(tar, ty->u.array.length);
        }
    }
    assert(0);
}

bool innerIdentifiers(S_symbol sym)
{
    if(sym == S_Symbol("int") || sym == S_Symbol("string") || sym == S_Symbol("char") || sym == S_Symbol("float") || sym == S_Symbol("void"))
    {
        return TRUE;
    }
    return FALSE;
}