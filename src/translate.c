#include "translate.h"
#include "env.h"

const int FRAME_SIZE = 4*1024;

T_stm Tr_mergeExpList(Tr_expList list)
{
    Temp_label main = Temp_namedlabel("main");
    T_stm stm;
    if(!main) assert(0);
    stm = T_Label(Temp_namedlabel("begin"));//T_Jump(T_Name(main), Temp_LabelList(main, NULL));
    while(list!=NULL){
        stm = T_Seq(stm, Tr_unNx(list->head));
        list = list->tail;
    }
    return stm;
}

Tr_access Tr_allocLocal(Tr_frame frame, bool escape)
{
    F_access acc;
    acc = F_allocLocal(frame->frame, escape);
    return Tr_Access(frame, acc);
}

Tr_access Tr_Access(Tr_frame frame, F_access access)
{
    Tr_access acc = (Tr_access)checked_malloc(sizeof(*acc));
    acc->frame = frame;
    acc->access = access;
    return acc;
}

Tr_accesslist Tr_Accesslist(Tr_access head, Tr_accesslist tail)
{
    Tr_accesslist tmp = (Tr_accesslist)checked_malloc(sizeof(*tmp));
    tmp->head = head;
    tmp->tail = tail;
    return tmp;
}

Tr_frame Tr_newFrame(Tr_frame parent, Temp_label name, U_boolList formals)
{
    Tr_frame frame = (Tr_frame)checked_malloc(sizeof(*frame));
    frame->parent = parent;
    frame->frame = F_newframe(name, U_BoolList(TRUE, formals));
    return frame;
}

Tr_accesslist Tr_formals(Tr_frame frame)
{
    F_accesslist f_acc = frame->frame->formals;
    f_acc = f_acc->tail;
    Tr_accesslist tmp = NULL;
    while(f_acc)
    {
         tmp = Tr_Accesslist(Tr_Access(frame, f_acc->head), tmp);
         f_acc = f_acc->tail;
    }
    return tmp;
}

/*
Tr_frame Tr_outermorst()
{
    static Tr_frame tmp = NULL;
    if (!tmp)
    {
        tmp = (Tr_frame)checked_malloc(sizeof(*tmp));
        U_boolList b = U_BoolList(TRUE, NULL);
        tmp->frame = F_newframe(Temp_newlabel(), b);
        tmp->parent = NULL;
    }
    return tmp;
}
*/

//------------------------

void Tr_ClearAcces(Tr_accesslist list)
{
    Tr_accesslist tmp;
    tmp = list;
    while(list)
    {
        list = list->tail;
        tmp->head = NULL;
        tmp->tail = NULL;
        free(tmp);
        tmp = list;
    }
}

Tr_exp Tr_Ex(T_exp exp)
{
    Tr_exp tmp = (Tr_exp)checked_malloc(sizeof(*tmp));
    tmp->kind = Tr_ex;
    tmp->u.ex = exp;
    return tmp;
}

Tr_exp Tr_Nx(T_stm stm)
{
    Tr_exp tmp = (Tr_exp)checked_malloc(sizeof(*tmp));
    tmp->kind = Tr_nx;
    tmp->u.nx = stm;
    return tmp;
}

Tr_exp Tr_Cx(patchList trues, patchList falses, T_stm stm)
{
    Tr_exp tmp = (Tr_exp)checked_malloc(sizeof(*tmp));
    tmp->kind = Tr_cx;
    tmp->u.cx.falses = falses;
    tmp->u.cx.trues = trues;
    tmp->u.cx.stm = stm;
    return tmp;
}

T_stm Tr_unNx(Tr_exp exp)
{
    switch (exp->kind)
    {
    case Tr_ex:
        return T_Exp(exp->u.ex);
    case Tr_cx:
        return T_Exp(Tr_unEx(exp));
    case Tr_nx:
        return exp->u.nx;
    }
    assert(0);
}

struct Cx Tr_unCx(Tr_exp exp)
{
    switch (exp->kind)
    {
    case Tr_cx:
        return exp->u.cx;
    case Tr_ex:
        {
            T_stm stm = T_Cjump(T_ne, exp->u.ex, T_Const(0), NULL, NULL);
            struct Cx cx; 
            cx.stm = stm;
            cx.falses = PatchList(&stm->u.CJUMP.falses, NULL);
            cx.trues = PatchList(&stm->u.CJUMP.trues, NULL);
            return cx;
        }
    }
    assert(0);
}

T_exp Tr_unEx(Tr_exp exp)
{
    switch(exp->kind)
    {
    case Tr_ex:
        return exp->u.ex;
    case Tr_cx :
        {
            Temp_temp tmp = Temp_newtemp();
            Temp_label t = Temp_newlabel(), f = Temp_newlabel();
            doPatch(exp->u.cx.falses, f);
            doPatch(exp->u.cx.trues, t);
            return T_Eseq(T_Move(T_Temp(tmp), T_Const(1)),
                    T_Eseq(exp->u.cx.stm,
                     T_Eseq(T_Label(f),
                      T_Eseq(T_Move(T_Temp(tmp), T_Const(0)),
                       T_Eseq(T_Label(t),
                        T_Temp(tmp))))));
        }
    case Tr_nx :
          return T_Eseq(exp->u.nx, T_Const(0));
    }
    assert(0);
}

void doPatch(patchList patchlist, Temp_label label)
{
    for (; patchlist; patchlist = patchlist->tail)
    {
        (*patchlist->head) = label;
    }
}

patchList PatchList(Temp_label *head , patchList patchlist ) 
{
    patchList tmp = (patchList)checked_malloc(sizeof(*tmp));
    tmp->head = head;
    tmp->tail = patchlist;
    return tmp;
}

Tr_expList Tr_ExpList(Tr_exp head , Tr_expList tail)
{
    Tr_expList tmp = (Tr_expList)checked_malloc(sizeof(*tmp));
    tmp->head = head;
    tmp->tail = tail;
    return tmp;
}

Tr_exp Tr_nilExp()
{
     return Tr_Nx(T_Exp(T_Const(0)));
}

Tr_exp Tr_intExp(int i)
{
    return Tr_Ex(T_Const(i));
}

Tr_exp Tr_floatExp(float f)
{
    return Tr_Ex(T_Const(*(int*)&f));
}

Tr_exp Tr_charExp(char c)
{
    return Tr_Ex(T_Const(c));
}

static F_fragList stringFragList = NULL;
Tr_exp Tr_stringExp(string s) 
{ 
    Temp_label slabel = Temp_newlabel();
    F_frag fragment = F_StringFrag(slabel, s);
    stringFragList = F_FragList(fragment, stringFragList);
    return Tr_Ex(T_Name(slabel));
}

Tr_exp Tr_simpleVar(Tr_access acc, Tr_frame frame)
{
    T_exp tmp = T_Temp(F_FP());
    while(frame && frame!=acc->frame->parent)
    {
        tmp = T_Mem(T_Binop(T_plus, T_Const(frame->frame->formals->head->u.offset), tmp));
        frame = frame->parent;
    }
    return Tr_Ex(F_Exp(acc->access, tmp));
}

Tr_exp Tr_subscriptVar(Tr_exp base, Tr_exp index)
{
    return Tr_Ex(T_Mem(T_Binop(T_plus, Tr_unEx(base), T_Binop(T_mul, Tr_unEx(index), T_Const(F_wordSize)))));
}

Tr_exp Tr_fieldVar(Tr_exp base, int offset)
{
    return Tr_Ex(T_Mem(T_Binop(T_plus, Tr_unEx(base), T_Const(offset*F_wordSize))));
}

Tr_exp Tr_addressVar(Tr_access acc, Tr_frame frame)
{
    T_exp tmp = T_Temp(F_FP());
    while(frame && frame!=acc->frame->parent)
    {
        tmp = T_Mem(T_Binop(T_plus, T_Const(frame->frame->formals->head->u.offset), tmp));
        frame = frame->parent;
    }
    return Tr_Ex(F_AddressExp(acc->access, tmp));
}

Tr_exp Tr_binopExp(Tr_exp left, Tr_exp right, A_oper oper)
{
    T_binOp op;
    switch(oper)
    {
    case A_plusOp:
        op = T_plus;
        break;
    case A_minusOp:
        op = T_minus;
        break;
    case A_timesOp:
        op = T_mul;
        break;
    case A_divideOp:
        op = T_div;
        break;
    case A_modOp:
        op = T_mod;
        break;
    case A_leftOp:
        op = T_lshift;
        break;
    case A_rightOp:
        op = T_rshift;
        break;
    case A_andOp:
        op = T_and;
        break;
    case A_orOp:
        op = T_or;
        break;
    case A_eorOp:
        op = T_xor;
        break;
    default :
        return NULL;
    }
   return Tr_Ex(T_Binop(op, Tr_unEx(left), Tr_unEx(right)));
}

Tr_exp Tr_relopExp(Tr_exp left, Tr_exp right, A_oper oper) 
{
    T_relOp op;
    switch(oper)
    {
    case A_landOp:
        return Tr_Ex(T_Binop(T_and, Tr_unEx(left), Tr_unEx(right)));
    case A_lorOp:
        return Tr_Ex(T_Binop(T_or, Tr_unEx(left), Tr_unEx(right)));
    case A_ltOp:
        op = T_lt;
        break;
    case A_leOp:
        op = T_le;
        break;
    case A_gtOp:
        op = T_gt;
        break;
    case A_geOp:
        op = T_ge;
        break;
    case A_eqOp:
        op = T_eq;
        break;
    case A_neqOp:
        op = T_ne;
        break;
    default :
        return NULL;
    }
    T_stm stm = T_Cjump(op, Tr_unEx(left), Tr_unEx(right), NULL, NULL);
    patchList trues = PatchList(&stm->u.CJUMP.trues , NULL);
    patchList falses = PatchList(&stm->u.CJUMP.falses , NULL);
    return Tr_Cx(trues, falses, stm);
}

Tr_exp Tr_unaryopExp(Tr_exp exp, A_unoper unoper)
{
    switch(unoper)
    {
    case A_notOp:
        return Tr_Ex(T_Binop(T_xor, T_Const(1), Tr_unEx(exp)));
    case A_bnotOp:
        return Tr_Ex(T_Binop(T_xor, T_Const(0xffffffff), Tr_unEx(exp)));
    default:
        return NULL;
    }
}

Tr_exp Tr_conExp(Tr_exp left, Tr_exp mid, Tr_exp right)
{
    struct Cx ctest = Tr_unCx(left);
    T_stm midexp, rightexp;
    Temp_label t = Temp_newlabel();
    Temp_label f = Temp_newlabel();
    Temp_label jump = Temp_newlabel();
    Temp_temp v = Temp_newtemp(); 
    doPatch(ctest.falses, f);
    doPatch(ctest.trues, t); 
    switch(mid->kind)
    {
    case Tr_nx:
        return NULL;
    case Tr_cx: case Tr_ex:
        midexp = T_Seq(ctest.stm, T_Seq(T_Label(t), T_Move(T_Temp(v), Tr_unEx(mid))));
        break;
    }
    switch(right->kind)
    {
    case Tr_nx:
        return NULL;
    case Tr_cx: case Tr_ex:
        rightexp = T_Seq(T_Jump(T_Name(jump), Temp_LabelList(jump, NULL)), T_Seq(T_Label(f), T_Move(T_Temp(v), Tr_unEx(right))));
        break;
    }
    return Tr_Ex(T_Eseq(T_Seq(midexp, T_Seq(rightexp, T_Label(jump))), T_Temp(v)));
}

Tr_exp Tr_ifExp(Tr_exp test, Tr_exp then, Tr_exp elses) 
{
    struct Cx ctest = Tr_unCx(test);
    T_stm reslutstm, thenstm, elsestm;
    reslutstm = thenstm = elsestm = NULL;
    Temp_label t = Temp_newlabel();
    Temp_label f = Temp_newlabel();
    Temp_temp v = Temp_newtemp(); 
    doPatch(ctest.falses, f);
    doPatch(ctest.trues, t); 
    switch(then->kind)
    {
    case Tr_ex:
        thenstm = T_Seq(ctest.stm, T_Seq(T_Label(t), T_Move(T_Temp(v), Tr_unEx(then))));
        break;
    case Tr_nx:
        thenstm = T_Seq(ctest.stm, T_Seq(T_Label(t), then->u.nx));
        break;
    case Tr_cx:   
        thenstm = T_Seq(ctest.stm, T_Seq(T_Label(t), Tr_unNx(then)));
        break;
    }
    if (elses)
    {
        switch(elses->kind)
        {
        case Tr_ex:
            elsestm = T_Move(T_Temp(v), Tr_unEx(elses));
            break;
        case Tr_nx :
            elsestm = elses->u.nx;
            break;
        case Tr_cx :
            elsestm = Tr_unNx(elses);
            break;
        }
    }
    if (elsestm)
    {
        Temp_label jump = Temp_newlabel();
        return Tr_Nx(T_Seq(T_Seq(thenstm, T_Seq(T_Jump(T_Name(jump), Temp_LabelList(jump, NULL)), T_Seq(T_Label(f), elsestm))), T_Label(jump)));
    }
    return Tr_Nx(T_Seq(thenstm, T_Label(f))); 
}

Tr_exp Tr_seqExp(Tr_expList explist)
{
    if(!explist)
    {
        return NULL;
    }
    T_stm stm = Tr_unNx(explist->head);
    explist = explist->tail;
    while(explist)
    {
        stm = T_Seq(Tr_unNx(explist->head), stm);
        explist = explist->tail;
    }
    return Tr_Nx(stm);
}

Tr_exp Tr_assignExp(Tr_exp dst, Tr_exp src)
{
    return Tr_Nx(T_Move(T_Mem(Tr_unEx(dst)), T_Mem(Tr_unEx(src))));
}

Tr_exp Tr_letExp(Tr_expList declist, Tr_exp exp)
{
    if(!declist)
    {
        return NULL;
    }
    T_stm stm = Tr_unNx(declist->head);
    declist = declist->tail;
    while(declist)
    {
        stm = T_Seq(Tr_unNx(declist->head), stm);
        declist = declist->tail;
    }
    if(exp)
    {
        stm = T_Seq(stm, Tr_unNx(exp));
    }
    return Tr_Nx(stm);
}

Tr_exp Tr_whileExp(Tr_exp test, Tr_exp body, bool isbreak)
{
    T_stm stm;
    Temp_label testlabel = Temp_newlabel();
    struct Cx ctest = Tr_unCx(test);
    Temp_label notdone = Temp_namedlabel("notdone");
    Temp_label done = Temp_namedlabel("done");
    doPatch(ctest.falses, done);
    doPatch(ctest.trues, notdone);
    stm = T_Seq(T_Label(notdone), T_Seq(Tr_unNx(body), T_Jump(T_Name(testlabel), Temp_LabelList(testlabel, NULL))));
    stm = T_Seq(T_Label(testlabel), T_Seq(ctest.stm, T_Seq(stm, T_Label(done))));
    return Tr_Nx(stm);
}

Tr_exp Tr_breakExp()
{
    Temp_label done = Temp_namedlabel("done");
    return Tr_Nx(T_Jump(T_Name(done), Temp_LabelList(done, NULL)));
}

Tr_exp Tr_continueExp()
{
    Temp_label notdone = Temp_namedlabel("notdone");
    return Tr_Nx(T_Jump(T_Name(notdone), Temp_LabelList(notdone, NULL)));
}

Tr_exp Tr_forExp(Tr_exp e1, Tr_exp e2, Tr_exp e3, Tr_exp body)
{
    T_stm stm;
    Temp_label testlabel = Temp_newlabel();
    struct Cx ctest = Tr_unCx(e2);
    Temp_label notdone = Temp_namedlabel("notdone");
    Temp_label done = Temp_namedlabel("done");
    doPatch(ctest.falses, done);
    doPatch(ctest.trues, notdone);
    stm = T_Seq(T_Label(notdone), T_Seq(Tr_unNx(body), T_Seq(Tr_unNx(e3), T_Jump(T_Name(testlabel), Temp_LabelList(testlabel, NULL)))));
    stm = T_Seq(Tr_unNx(e1), T_Seq(T_Label(testlabel), T_Seq(ctest.stm, T_Seq(stm, T_Label(done)))));
    return Tr_Nx(stm);
}

Tr_exp Tr_caseExp(Tr_exp test, Tr_exp constant, Tr_exp body)
{
    if(constant){
        Temp_label done = Temp_namedlabel("done");
        Temp_label t = Temp_newlabel();
        Temp_label f = Temp_newlabel();
        T_stm stm = T_Cjump(T_eq, Tr_unEx(test), Tr_unEx(constant), NULL, NULL);
        struct Cx cx; 
        cx.stm = stm;
        cx.falses = PatchList(&stm->u.CJUMP.falses, NULL);
        cx.trues = PatchList(&stm->u.CJUMP.trues, NULL);
        doPatch(cx.falses, f);
        doPatch(cx.trues, t); 
        stm = T_Seq(stm, T_Seq(T_Label(t), T_Seq(Tr_unNx(body), T_Jump(T_Name(done), Temp_LabelList(done, NULL)))));
        stm = T_Seq(stm, T_Label(f));
        return Tr_Nx(stm);
    }
    else{
        return Tr_Nx(Tr_unNx(body));
    }
}

Tr_exp Tr_switchExp(Tr_expList bodyList)
{
    Temp_label done = Temp_namedlabel("done");
    T_stm stm = T_Label(done);
    while(bodyList != NULL){
        stm = T_Seq(Tr_unNx(bodyList->head), stm);
        bodyList = bodyList->tail;
    }
    return Tr_Nx(stm);
}

Tr_exp Tr_returnExp(Tr_exp res)
{
    T_exp tmp = T_Temp(F_RV());
    T_stm stm = T_Move(T_Temp(F_FP()), T_Mem(T_Binop(T_plus, T_Temp(F_FP()), T_Const(0))));
    if(res)
        return Tr_Nx(T_Seq(T_Move(tmp, Tr_unEx(res)), T_Seq(stm, T_Ret())));
    else
        return Tr_Nx(T_Seq(stm, T_Ret()));
}

Tr_exp Tr_varDec(Tr_accesslist accList, Tr_expList initList)
{
    T_stm stm;// = Tr_unNx(Tr_Ex(T_Const(0)));
    T_exp tmp = T_Temp(F_FP());
    T_exp memt; 
    int flag = 0;
    while(accList != NULL && initList != NULL){
        memt = F_Exp(accList->head->access, tmp);
        if(initList->head){
            if(flag)
                stm = T_Seq(stm, T_Move(memt, Tr_unEx(initList->head)));
            else{
                stm = T_Move(memt, Tr_unEx(initList->head));
                flag = 1;
            }
        }
        accList = accList->tail;
        initList = initList->tail;
    }
    if(flag==0){
        return Tr_Ex(T_Const(0));
    }
    return Tr_Nx(stm);
}

Tr_exp Tr_structDec(Tr_access acc, Tr_exp init)
{
    return Tr_Ex(T_Const(0));
}

Tr_exp Tr_funDec(Temp_label label, Tr_exp body)
{
    /*
    T_stm stm;
    stm = T_Move(T_Temp(F_RV()) , Tr_unEx(body));
    bodylist = bodylist->tail;
    while (bodylist)
    {
      stm = T_Seq(T_Move(T_Temp(F_RV()) , Tr_unEx(bodylist->head)) , stm);
    }
    */
    return Tr_Nx(T_Seq(T_Label(label), T_Seq(T_Move(T_Temp(F_FP()), T_Binop(T_plus, T_Temp(F_FP()), T_Const(FRAME_SIZE))), Tr_unNx(body))));
}

Tr_exp Tr_callExp(Temp_label label, Tr_expList explist)
{
    T_expList tmplist = NULL;
    while(explist)
    {
        tmplist = T_ExpList(Tr_unEx(explist->head), tmplist);
        explist = explist->tail;
    }
    tmplist = T_ExpList(T_Temp(F_FP()), tmplist); //静态链为frame的FP
    T_exp callfun = T_Call(T_Name(label), tmplist); //call修改FP
    return Tr_Ex(callfun);
}

/*
 Tr_exp Tr_StaticLink(Tr_frame now, Tr_frame def)
 {
    T_exp addr = T_Temp(F_FP());
    while(now && (now != def->parent)) 
    { 
        F_access sl = F_formals(now->frame)->head;
        addr = F_Exp(sl, addr); //当前栈帧的静态链，即当前栈帧的FP
        now = now->parent;
    }
    return Tr_Ex(addr); //从now往前得到def的FP
}
*/


/*
Tr_exp Tr_callExp(E_enventry entry , Tr_frame frame , Tr_expList explist  ) 
{
    
     T_expList tmplist = NULL;
     while(explist)
     {
         tmplist = T_ExpList( Tr_unEx(explist->head) , tmplist);
         explist = explist->tail;
     }
     tmplist = T_ExpList(Tr_unEx(Tr_StaticLink(entry->u.fun.frame , frame)) , tmplist);
    T_exp callfun = T_Call(T_Name(entry->u.fun.label) ,tmplist);
    return Tr_Ex(callfun);
}
*/
/*
Tr_exp Tr_recordExp( Tr_expList explist , int num)  //’‚¿Ô¥´Ω¯¿¥µƒexplist ∫Õ’Ê «µƒexplist «œ‡∑¥µƒ
{
   T_stm stm;
   Temp_temp t = Temp_newtemp();
   T_exp callfun = T_Call(T_Name(Temp_namedlabel("initRecord")) ,T_ExpList(T_Const(num*F_wordSize) , NULL));

   stm = T_Move(T_Mem(T_Binop(T_plus , T_Temp(t) , T_Const((num - 1)*F_wordSize))), Tr_unEx(explist->head));
   num--;
   explist = explist->tail;
   while(explist)
   {
     stm = T_Seq(T_Move(T_Mem(T_Binop(T_plus, T_Temp(t) , T_Const((num -1 ) * F_wordSize))), Tr_unEx(explist->head)),stm);
     explist = explist->tail;
     num--;
   }
   stm = T_Seq(T_Move(T_Temp(t) , callfun) , stm);
   return Tr_Ex( T_Eseq( stm , T_Temp(t)));
}


Tr_exp Tr_arryExp(Tr_exp index , Tr_exp init)
{
    Temp_temp t = Temp_newtemp();
    T_exp callfun = T_Call(T_Name(Temp_namedlabel("initArray")) ,T_ExpList( Tr_unEx(index),T_ExpList(Tr_unEx(init) ,NULL)));
    return  Tr_Nx( T_Move( T_Temp(t) , callfun) );
}
*/