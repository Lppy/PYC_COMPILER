#include "translate.h"
#include "util.h"
#include "env.h"
#include <stdio.h>
#include <stdlib.h>
Tr_access Tr_allocLocal(Tr_level level , bool escape )
{
  F_access acc ;
  acc = F_allocLoacl(level->frame  , escape) ;
  return Tr_Access(level , acc) ;
}

Tr_access Tr_Access(Tr_level level , F_access access )
{
    Tr_access acc = (Tr_access)checked_malloc(sizeof(*acc));
    acc->level = level;
    acc->access = access;
    return acc;
}

Tr_accesslist Tr_Accesslist(Tr_access head , Tr_accesslist tial)
{
    Tr_accesslist tmp = (Tr_accesslist)checked_malloc(sizeof(*tmp));
    tmp->head = head ;
    tmp->tial = tial ;
    return tmp ;
}
Tr_level Tr_newLevel(Tr_level parent , Temp_label name , U_boolList formals )
{
    Tr_level level = (Tr_level)checked_malloc(sizeof(*level));
    level->parent = parent ;
    level->frame = F_newframe(name , U_BoolList(true , formals) ) ; // ’‚¿Ôº”µƒtrue¥˙±Ì¡Àæ≤Ã¨¡¥
    return level ;
}


Tr_accesslist Tr_formals(Tr_level level)
{
    F_accesslist f_acc = level->frame->formals ;
    f_acc = f_acc->tail ;//µ⁄“ª∏ˆ¥˙±Ì¡Àæ≤Ã¨¡¥ À?“‘“™ÃÙ≥ˆ¿¥
    Tr_accesslist tmp = NULL ;
    while(f_acc)
    {
         tmp = Tr_Accesslist(Tr_Access(level , f_acc->head), tmp) ;
         f_acc = f_acc->tail ;
    }
    return tmp ;
}

Tr_level Tr_outermorst()
{
    static Tr_level tmp = NULL ;
    if (!tmp)
    {
        tmp = (Tr_level)checked_malloc(sizeof(*tmp));
        U_boolList b = U_BoolList(true , NULL) ;
        tmp->frame = F_newframe(Temp_newlabel() , b)  ;
        tmp->parent = NULL ;
    }
    return tmp ;

}

void Tr_ClearAcces(Tr_accesslist list)
{
    Tr_accesslist tmp ;
    tmp = list ;
    while(list)
    {
        list = list->tial ;
        tmp->head =  NULL ;
        tmp->tial =  NULL ;
        free(tmp) ;
        tmp = list ;
    }
}

Tr_exp Tr_Ex(T_exp exp )
{
    Tr_exp tmp = (Tr_exp)checked_malloc(sizeof(*tmp));
    tmp->kind = GetExpEx ;
    tmp->u.ex = exp ;
    return tmp ;
}

Tr_exp Tr_Nx(T_stm stm )
{
    Tr_exp tmp = (Tr_exp)checked_malloc(sizeof(*tmp));
    tmp->kind = GetExpNx ;
    tmp->u.nx = stm ;
    return tmp ;
}

Tr_exp Tr_Cx(patchList trues , patchList falses , T_stm stm )
{
    Tr_exp tmp = (Tr_exp)checked_malloc(sizeof(*tmp));
    tmp->kind = GetExpCx ;
    tmp->u.cx.falses = falses ;
    tmp->u.cx.trues = trues ;
    tmp->u.cx.stm = stm ;
    return tmp ;
}
T_stm  Tr_unNx(Tr_exp exp)
{
    switch (exp->kind)
    {
    case  GetExpEx:
        return T_Exp(exp->u.ex);
    case  GetExpCx:
        return T_Exp(Tr_unEx(exp));
    case  GetExpNx:
        return exp->u.nx;
    }
    assert(0);
}
Cx  Tr_unCx(Tr_exp exp)
{
    switch (exp->kind)
    {
    case GetExpCx :
        return exp->u.cx;
    case GetExpEx:
        {
            T_stm stm = T_Cjump(T_ne, exp->u.ex, T_Const(0), NULL ,  NULL);
            Cx cx; 
            cx.stm = stm;
            cx.falses = PatchList(&stm->u.CJUMP.falses, NULL);
            cx.trues = PatchList(&stm->u.CJUMP.trues, NULL);
            return cx;
        }
    }
    assert(0);
}
T_exp  Tr_unEx(Tr_exp exp)
{
    switch(exp->kind)
    {
    case GetExpEx :
     return exp->u.ex ;
    case GetExpCx :
        {
          Temp_temp tmp = Temp_newtemp() ;
          Temp_label t = Temp_newlabel() , f = Temp_newlabel() ;
          doPatch(exp->u.cx.falses , f) ;
          doPatch(exp->u.cx.trues , t) ;
          return T_Eseq(T_Move(T_Temp(tmp) , T_Const(1)),
                  T_Eseq( exp->u.cx.stm ,
                   T_Eseq( T_Label(f) ,
                    T_Eseq(T_Move(T_Temp(tmp) , T_Const(0)) ,
                     T_Eseq(T_Label(t) ,
                      T_Temp(tmp)))))) ;

        }
    case GetExpNx :
        {
          return T_Eseq(exp->u.nx, T_Const(0));// Œ™ ≤√¥“™∑µªÿ0 ’‚∏ˆnx≤ª «Œ?÷µ”Ôæ‰√¥
        }
    }
    assert(0);
}

void doPatch(  patchList patchlist , Temp_label lable)
{
    for ( ;  patchlist ; patchlist = patchlist->tail)
    {
        (*patchlist->head) = lable ;
    }
}

patchList PatchList(Temp_label *head , patchList patchlist ) 
{
    patchList tmp = (patchList)checked_malloc(sizeof(*tmp));
    tmp->head = head ;
    tmp->tail = patchlist ;
    return tmp ;
}

Tr_expList Tr_ExpList(Tr_exp head , Tr_expList tail)
{
    Tr_expList tmp = (Tr_expList)checked_malloc(sizeof(*tmp)) ;
    tmp->head = head ;
    tmp->tail = tail ;
    return tmp ;
}

Tr_exp Tr_nilExp()
{
    static Temp_temp temp =  NULL  ;
        if ( !temp )
        {
           temp = Temp_newtemp() ;
           T_stm alloc = T_Move(T_Temp(temp),
               T_Call(T_Name(Temp_namedlabel("initRecord")), T_ExpList(T_Const(0), NULL)));
           return Tr_Ex(T_Eseq(alloc, T_Temp(temp)));
        }

     return Tr_Ex(T_Temp(temp)) ;
}

Tr_exp Tr_intExp(int i)
{
    return  Tr_Ex(T_Const(i)) ;
}

static F_fragList stringFragList = NULL;
Tr_exp Tr_stringExp(string s) 
{ 
    Temp_label slabel = Temp_newlabel();
    F_frag fragment = F_StringFrag(slabel, s);
    stringFragList = F_FragList(fragment, stringFragList);
    return Tr_Ex(T_Name(slabel));
}

Tr_exp Tr_simpleVar(Tr_access acc, Tr_level level)
{
    T_exp tmp = T_Temp(F_FP());
    while ( level && level != acc->level->parent )
    {
        tmp = T_Mem(T_Binop(T_plus, T_Const(level->frame->formals->head->u.offset), tmp));
        level = level->parent;
    }
    
    return  Tr_Ex(F_Exp(acc->access, tmp));
}

Tr_exp Tr_subscriptVar(Tr_exp base, Tr_exp index )
{
    return Tr_Ex(T_Mem(T_Binop(T_plus, Tr_unEx(base), T_Binop(T_mul, Tr_unEx(index), T_Const(F_wordSize)))));
}

Tr_exp Tr_fieldVar(Tr_exp base, int offset)
{
    return Tr_Ex(T_Mem(T_Binop(T_plus, Tr_unEx(base), T_Const(offset * F_wordSize))));
}

Tr_exp Tr_binopExp(Tr_exp left , Tr_exp right ,A_oper oper )
{
    T_binOp op ;
    switch(oper)
    {
    case A_plusOp   :
        {
            op = T_binOp::T_plus ;
            break ;
        }
    case A_minusOp  :
        {
            op = T_binOp::T_minus ;
            break ;
        }
    case A_timesOp  :
        {
            op = T_binOp::T_mul ;
            break ;
        }
    case A_divideOp :
        {
            op = T_binOp::T_div ;
            break ;
        }
    default :
        {
            return NULL ;
        }
    }
   return Tr_Ex(T_Binop(op , Tr_unEx(left) , Tr_unEx(right))) ;
}
Tr_exp  Tr_relopExp(Tr_exp left , Tr_exp right , A_oper oper) 
{
    T_relOp op ;
    switch(oper)
    {
     case A_ltOp :
         {
             op = T_relOp::T_lt ;
             break ;
         }
     case A_leOp :
         {
             op = T_relOp::T_le ;
             break ;
         }
     case A_gtOp :
         {
             op = T_relOp::T_gt ;
             break ;
         }
     case A_geOp :
         {
              op = T_relOp::T_ge ;
             break ;
         }
     case A_eqOp :
         {
             op = T_relOp::T_eq ;
             break ;
         }
     case A_neqOp :
         {
             op = T_relOp::T_ne ;
             break ;
         }
     default :
         return NULL ;
    }
    T_stm stm = T_Cjump(op ,Tr_unEx(left) , Tr_unEx(right) , NULL , NULL );
    patchList trues =  PatchList(&stm->u.CJUMP.trues , NULL) ;
    patchList falses =  PatchList(&stm->u.CJUMP.falses , NULL) ;
    return  Tr_Cx(trues , falses , stm) ;
}

Tr_exp Tr_ifExp( Tr_exp test , Tr_exp then , Tr_exp elses ) 
{

   Cx ctest  = Tr_unCx(test) ;
   T_stm reslutstm , thenstm , elsestm  ;
   reslutstm = thenstm = elsestm = NULL ;
   Temp_label t = Temp_newlabel() ;
   Temp_label f = Temp_newlabel() ;
   Temp_temp v = Temp_newtemp() ; 
    doPatch(ctest.falses , f) ;
    doPatch(ctest.trues , t ) ; 
        switch(then->kind)
        {
        case GetExpEx :
            {
                thenstm = T_Seq(ctest.stm , T_Seq(T_Label(t) , T_Move(T_Temp(v) , Tr_unEx(then)))) ;
                break ;
            }
        case GetExpNx :
            {
                thenstm = T_Seq(ctest.stm ,T_Seq(T_Label(t) , then->u.nx)) ;
                break ;
            }
        case GetExpCx :
            {
                
                thenstm = T_Seq(ctest.stm , T_Seq(T_Label(t) , Tr_unNx(then))) ;
                break ;
            }
        }

   if (elses)
   {
       switch(elses->kind)
       {
       case GetExpEx:
           {
               elsestm = T_Move(T_Temp(v), Tr_unEx(elses)) ;
               break ;
           }
       case GetExpNx :
           {
               elsestm = elses->u.nx ;
               break ;
           }
       case GetExpCx :
           {
              elsestm = Tr_unNx(elses) ;
              break ;
           }
       }
   }
   if (elsestm)
   {
       Temp_label jump = Temp_newlabel() ;
       
       return Tr_Nx(T_Seq(
           T_Seq( thenstm , T_Seq(T_Jump( T_Name(jump) , Temp_LabelList(jump , NULL)) ,T_Seq(T_Label(f) ,elsestm))) ,
           T_Label(jump))) ;
   }
        return Tr_Nx( T_Seq(thenstm , T_Label(f))) ; 
}

Tr_exp Tr_recordExp( Tr_expList explist , int num)  //’‚¿Ô¥´Ω¯¿¥µƒexplist ∫Õ’Ê «µƒexplist «œ‡∑¥µƒ
{
   T_stm stm ;
   Temp_temp t = Temp_newtemp();
   T_exp callfun = T_Call(T_Name(Temp_namedlabel("initRecord")) ,T_ExpList(T_Const(num*F_wordSize) , NULL)) ;

   stm = T_Move(T_Mem(T_Binop(T_plus , T_Temp(t) , T_Const((num - 1)*F_wordSize))), Tr_unEx(explist->head)) ;
   num-- ;
   explist = explist->tail ;
   while(explist)
   {
     stm = T_Seq(T_Move(T_Mem(T_Binop(T_plus, T_Temp(t) , T_Const((num -1 ) * F_wordSize))), Tr_unEx(explist->head)),stm) ;
     explist = explist->tail ;
     num-- ;
   }
   stm = T_Seq(T_Move(T_Temp(t) , callfun) , stm) ;
   return Tr_Ex( T_Eseq( stm , T_Temp(t))) ;
}


Tr_exp Tr_arryExp(Tr_exp index , Tr_exp init)
{
    Temp_temp t = Temp_newtemp() ;
    T_exp callfun = T_Call(T_Name(Temp_namedlabel("initArray")) ,T_ExpList( Tr_unEx(index),T_ExpList(Tr_unEx(init) ,NULL))) ;
    return  Tr_Nx( T_Move( T_Temp(t) , callfun) );
}
 
Tr_exp Tr_seqExp(Tr_expList explist)
{
    if (!explist)
    {
        return NULL ;
    }
    T_stm stm = Tr_unNx(explist->head) ;
    explist = explist->tail ;
    while(explist)
    {
        stm = T_Seq( Tr_unNx(explist->head) , stm) ;
        explist = explist->tail ;
    }
    return Tr_Nx(stm) ;
}

Tr_exp Tr_assignExp(Tr_exp exp1 , Tr_exp exp2)
{
    return Tr_Nx(T_Move(T_Mem( Tr_unEx(exp1)) , T_Mem(Tr_unEx(exp2))))  ;
}

Tr_exp Tr_letExp(Tr_expList declist , Tr_exp exp )
{
    if (!declist)
    {
        return NULL ;
    }
    T_stm stm = Tr_unNx(declist->head) ;
    declist = declist->tail ;
    while(declist)
    {
        stm = T_Seq(Tr_unNx(declist->head) , stm) ;
        declist = declist->tail ;
    }
    if (exp)
    {
        stm = T_Seq(stm , Tr_unNx(exp)) ;
    }
    return Tr_Nx(stm);
}

Tr_exp Tr_whileExp(Tr_exp test , Tr_exp body , bool isbreak)
{
  
    Cx ctest = Tr_unCx(test) ;
    Temp_label t = Temp_newlabel() ;
    Temp_label done = Temp_namedlabel("done") ;
    doPatch(ctest.falses , done) ;
    doPatch(ctest.trues , t) ;
    T_stm  stm ;
    Temp_label testlabel = Temp_newlabel();
    
    stm = T_Seq(T_Label(t) , T_Seq(Tr_unNx(body) ,T_Jump(T_Name(testlabel),Temp_LabelList(testlabel,NULL)))) ;
    stm = T_Seq( T_Label(testlabel) , T_Seq(ctest.stm ,T_Seq(stm , T_Label(done)))) ;
    return Tr_Nx(stm);
}

Tr_exp Tr_breakExp()
{
    Temp_label done = Temp_namedlabel("done") ;
    return Tr_Nx(T_Jump(T_Name(done) , Temp_LabelList(done , NULL))) ;
}

Tr_exp Tr_forExp(Tr_access acc , Tr_exp hi , Tr_exp low , Tr_exp body)
{
    T_exp tmp = T_Temp(F_FP());
     //tmp = T_Mem(T_Binop(T_plus ,tmp , T_Const(acc->level->frame->formals->head->u.offset))) ;
      T_exp memt =  F_Exp(acc->access , tmp) ;
    T_stm stm = T_Move(memt , Tr_unEx(low)) ;
    Temp_temp h = Temp_newtemp() ;
    T_stm limit = T_Move(T_Temp(h) , Tr_unEx(hi)) ;
    Cx cx ;
    cx.stm =  T_Cjump(T_lt ,memt ,T_Temp(h) , NULL , NULL) ;
    cx.falses = PatchList(&cx.stm->u.CJUMP.falses , NULL) ;
    cx.trues = PatchList(&cx.stm->u.CJUMP.trues , NULL) ;
    T_stm finalbody = T_Seq(T_Move(memt , T_Binop(T_plus , memt , T_Const(1))) , Tr_unNx(body)) ;
    
    Tr_exp tmpwhile = Tr_whileExp(Tr_Cx(cx.trues , cx.falses , cx.stm) , Tr_Nx(finalbody) , false) ;
    return  Tr_Nx(T_Seq( stm , T_Seq( limit , Tr_unNx(tmpwhile)))) ;
}
 Tr_exp Tr_StaticLink(Tr_level now, Tr_level def)
 {
    T_exp addr = T_Temp(F_FP());
    while(now && (now != def->parent)) 
    { 
        F_access sl = F_formals(now->frame)->head;
        addr = F_Exp(sl, addr);
        now = now->parent;
    }
    return Tr_Ex(addr);
}
Tr_exp Tr_callExp(Temp_label label ,Tr_level funleve , Tr_level  level ,Tr_expList explist ) 
{
    T_expList tmplist = NULL;
    while(explist)
    {
        tmplist = T_ExpList( Tr_unEx(explist->head) , tmplist) ;
        explist = explist->tail ;
    }
    tmplist = T_ExpList(Tr_unEx(Tr_StaticLink(funleve , level)) , tmplist) ;
    T_exp callfun = T_Call(T_Name(label) ,tmplist) ;
    return Tr_Ex(callfun) ;
}
Tr_exp Tr_callExp(E_enventry entry , Tr_level level , Tr_expList explist  ) 
{
    
     T_expList tmplist = NULL;
     while(explist)
     {
         tmplist = T_ExpList( Tr_unEx(explist->head) , tmplist) ;
         explist = explist->tail ;
     }
     tmplist = T_ExpList(Tr_unEx(Tr_StaticLink(entry->u.fun.level , level)) , tmplist) ;
    T_exp callfun = T_Call(T_Name(entry->u.fun.label) ,tmplist) ;
    return Tr_Ex(callfun) ;
}

Tr_exp Tr_varDec(Tr_access acc , Tr_exp init)
{
    T_exp tmp = T_Temp(F_FP());
    T_exp memt =  F_Exp(acc->access , tmp) ;
    return Tr_Nx(T_Move(memt , Tr_unEx(init)))  ;
}

Tr_exp Tr_typeDec()
{
     return Tr_Ex(T_Const(0)) ;
}

Tr_exp Tr_funDec(Tr_expList bodylist)
{
    T_stm stm ;
    stm = T_Move(T_Temp(F_RV()) , Tr_unEx(bodylist->head)) ;
    bodylist = bodylist->tail ;
    while (bodylist)
    {
      stm = T_Seq(T_Move(T_Temp(F_RV()) , Tr_unEx(bodylist->head)) , stm);
    }
    return Tr_Nx(stm) ;
}