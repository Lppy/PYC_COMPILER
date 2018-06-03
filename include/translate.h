#ifndef TRANSLATE_H_
#define TRANSLATE_H_

#include "util.h"
#include "frame.h"
#include "absyn.h"
#include "tree.h"
  
typedef struct Tr_access_ *Tr_access;  
struct  Tr_access_ { Tr_level level; F_access access; };

typedef struct Tr_accesslist_ *Tr_accesslist ;
struct  Tr_accesslist_ { Tr_access head; Tr_accesslist tail; };

typedef struct Tr_level_ *Tr_level ;
struct  Tr_level_ { Tr_level parent; F_frame frame; };

struct Cx { patchList trues; patchList falses; T_stm stm; };

typedef struct Tr_exp_*Tr_exp;
struct Tr_exp_
{
    enum { Tr_ex, Tr_nx, Tr_cx } kind;
    union {
        T_exp ex;
        T_stm nx;
        struct Cx cx;
    } u;
};

typedef struct Tr_expList_ *Tr_expList;
struct Tr_expList_ { Tr_exp head; Tr_expList tail; };

typedef struct patchList_ *patchList;
struct patchList_ { Temp_label *head; patchList tail; };


Tr_exp Tr_Ex(T_exp exp);
Tr_exp Tr_Nx(T_stm stm);
Tr_exp Tr_Cx(patchList trues, patchList falses, T_stm stm);

T_exp  Tr_unEx(Tr_exp exp);
T_stm  Tr_unNx(Tr_exp exp);
struct Cx Tr_unCx(Tr_exp exp);

patchList PatchList(Temp_label *head , patchList patchlist);
void doPatch(patchList patchlist , Temp_label label);
//patchList joinPatch(patchList ftrst, patchList second);


Tr_exp Tr_nilExp(void);
Tr_exp Tr_intExp(int i);
Tr_exp Tr_floatExp(float f);
Tr_exp Tr_charExp(char c);
Tr_exp Tr_stringExp(string str);

Tr_exp Tr_simpleVar(Tr_access acc, Tr_level level);
Tr_exp Tr_subscriptVar(Tr_exp base, Tr_exp index);
Tr_exp Tr_fieldVar(Tr_exp base, int offset);
Tr_exp Tr_addressVar(Tr_access acc, Tr_level level);

Tr_exp Tr_binopExp(Tr_exp left, Tr_exp right, A_oper oper); //mod
Tr_exp Tr_relopExp(Tr_exp left, Tr_exp right, A_oper oper);
Tr_exp Tr_unaryopExp(Tr_exp exp, A_unoper unoper);

Tr_exp Tr_seqExp(Tr_expList explist);

Tr_exp Tr_conExp(Tr_exp left, Tr_exp mid, Tr_exp right);
Tr_exp Tr_ifExp(Tr_exp test, Tr_exp then, Tr_exp elsee);
Tr_exp Tr_assignExp(Tr_exp dst, Tr_exp src);
Tr_exp Tr_whileExp(Tr_exp test, Tr_exp body, bool isbreak);
Tr_exp Tr_forExp(Tr_exp e1, Tr_exp e2, Tr_exp e3, Tr_exp body);
Tr_exp Tr_breakExp();
Tr_exp Tr_continueExp();
Tr_exp Tr_letExp(Tr_expList declist, Tr_exp exp);
Tr_exp Tr_caseExp(Tr_exp test, Tr_exp constant, Tr_exp body);
Tr_exp Tr_switchExp(Tr_expList bodyList);  //从后往前的body链表
Tr_exp Tr_returnExp(Tr_exp res); //F_RA()

Tr_exp Tr_callExp(Temp_label label, Tr_level funlevel, Tr_level level, Tr_expList args);

Tr_exp Tr_varDec(Tr_accesslist accList, Tr_expList initList);
Tr_exp Tr_structDec();
Tr_exp Tr_funDec(Tr_exp body);


Tr_exp Tr_StaticLink(Tr_level now, Tr_level def);

Tr_expList Tr_ExpList(Tr_exp head , Tr_expList tail);
void Tr_FreeExpList(Tr_expList list);

void Tr_procEntryExit(Tr_level level, Tr_exp body, Tr_accesslist formals);

F_fragList Tr_getResult() ;

Tr_accesslist Tr_Accesslist(Tr_access head, Tr_accesslist tial);
Tr_access Tr_Access(Tr_level level, F_access access);
Tr_level Tr_outermorst();
Tr_level Tr_newLevel(Tr_level parent, Temp_label name, U_boolList formals);
Tr_accesslist Tr_formals(Tr_level level);
Tr_access Tr_allocLocal(Tr_level level, bool escape);
void Tr_ClearAcces(Tr_accesslist list);

//Tr_exp Tr_recordExp(Tr_expList explist , int num);
//Tr_exp Tr_recordExp( Tr_expList explist , int num);
//Tr_exp Tr_arryExp(Tr_exp index , Tr_exp init) ;

#endif