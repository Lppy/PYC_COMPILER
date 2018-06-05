#include "frame.h"

const int offset = -4;
const int F_wordSize = 4;

int alloc_bind = 1;

static Temp_temp fp = NULL;
Temp_temp F_FP()
{
    if(fp == NULL)
    {
        fp = Temp_newtemp();
    }
    return fp;
}

static Temp_temp rv = NULL;
Temp_temp F_RV()
{
    if(rv == NULL)
    {
        rv = Temp_newtemp();
    }
    return rv;
}

F_access F_allocLocal(F_frame f, bool escape)
{
    F_access access;
    if(escape == TRUE)
    {
        access = InFrame(f->framesize);
        f->framesize -= offset;
    }
    else
    {
        access = InReg(Temp_newtemp());
    }
    f->locals = F_Accesslist(access , f->locals);
    return access;
}

F_access InFrame(int offset)
{
    F_access tmp = (F_access) checked_malloc(alloc_bind*sizeof(*tmp));
    tmp->kind = inFrame;
    tmp->u.offset = offset;
    return tmp;
}

F_access InReg(Temp_temp reg)
{
    F_access tmp = (F_access) checked_malloc(sizeof(*tmp));
    tmp->kind = inReg;
    tmp->u.reg = reg;
    return tmp;
}

F_frame F_newframe(Temp_label name , U_boolList formals)
{
    F_frame frame =(F_frame) checked_malloc(sizeof(*frame)) ;
    frame->name = name ;
    frame->formals = NULL ;
    U_boolList par = formals ;
    F_access acc ;
    frame->framesize = 0 ;
    while(par != NULL)
    {
        if (par->head)
        {
          acc = InFrame(frame->framesize) ;   
          frame->framesize -= offset ;
        }
        else
        {
         acc = InReg(Temp_newtemp()) ;
        }
        frame->formals = F_Accesslist(acc , frame->formals);
        par = par->tail ;
    }
   frame->locals = NULL;
   return frame;
}

F_accesslist F_Accesslist(F_access head , F_accesslist tail)
{
    F_accesslist tmp = (F_accesslist)checked_malloc(sizeof(*tmp));
    tmp->head = head;
    tmp->tail = tail;
    return tmp;
}

T_exp F_AddressExp(F_access acc, T_exp framePtr)
{
    if(acc->kind == inFrame)
    {
        return T_Binop(T_plus, framePtr, T_Const(acc->u.offset));
    }
    return NULL;
}

T_exp F_Exp(F_access acc, T_exp framePtr)
{
    if (acc->kind == inFrame)
    {
        return T_Mem(T_Binop(T_plus, framePtr, T_Const(acc->u.offset)));
    }
    return  T_Temp(acc->u.reg);
}

F_frag F_StringFrag(Temp_label label , string str)
{
    F_frag tmp = (F_frag) checked_malloc(sizeof(*tmp)) ;
    tmp->kind = F_stringFrag ;
    tmp->u.stringg.label = label ;
    tmp->u.stringg.str = str ;
    return tmp ;
}

F_frag F_ProcFrag( T_stm stm , F_frame frame )
{
    F_frag tmp = (F_frag) checked_malloc(sizeof(*tmp)) ;
    tmp->kind = F_procFrag ;
    tmp->u.proc.body = stm ;
    tmp->u.proc.frame = frame ;
    return tmp ;
}

F_fragList F_FragList(F_frag frag , F_fragList tail)
{
    F_fragList tmp = (F_fragList) checked_malloc(sizeof(*tmp)) ;
    tmp->head = frag ;
    tmp->tail = tail ;
    return tmp; 
}

T_stm F_procEntryExit1(F_frame frame, T_stm stm) 
{
    return stm ;
}


F_accesslist F_formals(F_frame frame)
{
    return frame->formals;
}
