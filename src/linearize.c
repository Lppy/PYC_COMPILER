#include "linearize.h"

struct stmExp {T_stm s; T_exp e;};

typedef struct expRefList_ *expRefList;
struct expRefList_ {T_exp *head; expRefList tail;};

static T_stm do_stm(T_stm stm);
static struct stmExp do_exp(T_exp exp);

static expRefList ExpRefList(T_exp *head, expRefList tail)
{
	expRefList p = (expRefList)checked_malloc(sizeof *p);
	p->head=head; p->tail=tail;
	return p;
}

static bool isNop(T_stm x) 
{
	return x->kind == T_EXP && x->u.EXP->kind == T_CONST;
}

static T_stm seq(T_stm x, T_stm y)
{
	if (isNop(x)) return y;
	if (isNop(y)) return x;
	return T_Seq(x,y);
}

static bool commute(T_stm x, T_exp y)
{
    if (isNop(x)) return TRUE;
    if (y->kind == T_NAME || y->kind == T_CONST) return TRUE;
    return FALSE;
}



static T_stm reorder(expRefList rlist) {
    if (!rlist) 
        return T_Exp(T_Const(0)); /* nop */
    else if ((*rlist->head)->kind==T_CALL) {
        Temp_temp t = Temp_newtemp();
        *rlist->head = T_Eseq(T_Move(T_Temp(t),*rlist->head),T_Temp(t));
        return reorder(rlist);
    }
    else {
        struct stmExp hd = do_exp(*rlist->head);
        T_stm s = reorder(rlist->tail);
        if (commute(s,hd.e)) {
            *rlist->head=hd.e;
            return seq(hd.s,s);
        } else {
            Temp_temp t = Temp_newtemp();
            *rlist->head=T_Temp(t);
            return seq(hd.s, seq(T_Move(T_Temp(t),hd.e), s));
        }
    }
}

static expRefList get_call_rlist(T_exp exp)
{
    expRefList rlist, curr;
    T_expList args = exp->u.CALL.args;
    curr = rlist = ExpRefList(&exp->u.CALL.fun, NULL);
    for (; args; args=args->tail) {
        curr = curr->tail = ExpRefList(&args->head, NULL);
    }
    return rlist;
}

static struct stmExp StmExp(T_stm stm, T_exp exp) 
{
    struct stmExp x;
    x.s = stm;
    x.e = exp;
    return x;
}

static struct stmExp do_exp(T_exp exp)
{
    switch(exp->kind) {
    case T_BINOP: 
        return StmExp(reorder(ExpRefList(&exp->u.BINOP.left, 
        ExpRefList(&exp->u.BINOP.right, NULL))),
        exp);
    case T_MEM: 
        return StmExp(reorder(ExpRefList(&exp->u.MEM, NULL)), exp);
    case T_ESEQ:
    {
        struct stmExp x = do_exp(exp->u.ESEQ.exp);
        return StmExp(seq(do_stm(exp->u.ESEQ.stm), x.s), x.e);
    }
    case T_CALL:    
        return StmExp(reorder(get_call_rlist(exp)), exp);
    default:
        return StmExp(reorder(NULL), exp);
    }
}

static T_stm do_stm(T_stm stm)
{
    switch (stm->kind) {
    case T_SEQ: 
        return seq(do_stm(stm->u.SEQ.left), do_stm(stm->u.SEQ.right));
    case T_JUMP:
        return seq(reorder(ExpRefList(&stm->u.JUMP.exp, NULL)), stm);
    case T_CJUMP:
        return seq(reorder(ExpRefList(&stm->u.CJUMP.left, 
        ExpRefList(&stm->u.CJUMP.right,NULL))), stm);
    case T_MOVE:
        if (stm->u.MOVE.dst->kind == T_TEMP && stm->u.MOVE.src->kind == T_CALL)
            return seq(reorder(get_call_rlist(stm->u.MOVE.src)), stm);
        else if (stm->u.MOVE.dst->kind == T_TEMP)
            return seq(reorder(ExpRefList(&stm->u.MOVE.src, NULL)), stm);
        else if (stm->u.MOVE.dst->kind == T_MEM)
            return seq(reorder(ExpRefList(&stm->u.MOVE.dst->u.MEM, 
            ExpRefList(&stm->u.MOVE.src, NULL))), stm);
        else if (stm->u.MOVE.dst->kind == T_ESEQ) {
            T_stm s = stm->u.MOVE.dst->u.ESEQ.stm;
            stm->u.MOVE.dst = stm->u.MOVE.dst->u.ESEQ.exp;
            return do_stm(T_Seq(s, stm));
        }
        assert(0); /* dst should be temp or mem only */
    case T_EXP:
        if (stm->u.EXP->kind == T_CALL)
            return seq(reorder(get_call_rlist(stm->u.EXP)), stm);
        else 
            return seq(reorder(ExpRefList(&stm->u.EXP, NULL)), stm);
    default:
        return stm;
    }
}

static T_stmList linear(T_stm stm, T_stmList right)
{
    if (stm->kind == T_SEQ) 
        return linear(stm->u.SEQ.left,linear(stm->u.SEQ.right,right));
    else 
        return T_StmList(stm, right);
}

T_stmList linearize(T_stm stm)
{
    return linear(do_stm(stm), NULL);
}