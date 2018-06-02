#ifndef TRANSLATE_H_
#define TRANSLATE_H_

#include "frame.h"
#include "absyn.h"
//frame œ‡πÿ

typedef struct Tr_level_      * Tr_level ;    
typedef struct Tr_accesslist_ * Tr_accesslist ;
typedef struct Tr_access_     * Tr_access ;  
struct  Tr_accesslist_  { Tr_access  head ;  Tr_accesslist tial ; };
struct  Tr_level_       { Tr_level parent ;  F_frame frame; };  //≤„µƒ◊˜”√ «Œ™¡À±£¥Êæ≤Ã¨¡¥
struct  Tr_access_      { Tr_level level  ;  F_access access ; };//º«¬º¡À“ª∏ˆ≤„∫Õ≤„÷–µƒŒª÷√ ’‚—? ∑Ω±„“ª∏ˆ«∂Ã◊≤„»•≤È—Ø“?”√µƒÕ‚≤ø±‰¡ø À¸ «E_varEntry÷–µƒ“ª∏ˆ±‰¡ø


//frame œ‡πÿ
Tr_accesslist Tr_Accesslist(Tr_access head , Tr_accesslist tial) ;
Tr_access Tr_Access(Tr_level level , F_access access ) ;
Tr_level Tr_outermorst() ;
Tr_level Tr_newLevel(Tr_level parent , Temp_label name , U_boolList formals ) ;
Tr_accesslist Tr_formals(Tr_level level) ;
Tr_access Tr_allocLocal(Tr_level level , bool escape ) ;
void     Tr_ClearAcces(Tr_accesslist list) ; //Ω´list Õ∑≈ µ´ « ≤ª Õ∑≈?‰÷–µƒhead



// IR Tree œ‡πÿ
#define  GetExpEx  Tr_exp_::Tr_ex
#define  GetExpNx  Tr_exp_::Tr_nx 
#define  GetExpCx  Tr_exp_::Tr_cx 

typedef struct Tr_exp_* Tr_exp ;
typedef struct Tr_expList_ * Tr_expList ;
typedef struct patchList_ * patchList ;

struct Cx { patchList trues ; patchList falses ; T_stm stm ; };
struct patchList_ { Temp_label *head ; patchList tail ; };

struct Tr_exp_
{
    enum{Tr_ex , Tr_nx , Tr_cx } kind;
    union
    {
        T_exp ex ;
        T_stm nx ;
        Cx    cx ;
    }u;
};

struct Tr_expList_
{
    Tr_exp head ;
    Tr_expList tail ;
};

//IR Treeœ‡πÿ 
Tr_exp Tr_Ex(T_exp exp ) ;
Tr_exp Tr_Nx(T_stm stm ) ;
Tr_exp Tr_Cx(patchList trues , patchList falses , T_stm stm ) ;

T_exp  Tr_unEx(Tr_exp exp) ;
T_stm  Tr_unNx(Tr_exp exp) ;
Cx     Tr_unCx(Tr_exp exp) ;

patchList PatchList(Temp_label *head , patchList patchlist ) ;
void doPatch(patchList patchlist , Temp_label label) ;
Tr_expList Tr_ExpList(Tr_exp head , Tr_expList tail) ;

Tr_exp  Tr_nilExp() ;
Tr_exp  Tr_intExp(int i) ;
Tr_exp  Tr_stringExp(string str) ;

Tr_exp  Tr_simpleVar(Tr_access acc , Tr_level level );
Tr_exp  Tr_subscriptVar(Tr_exp base, Tr_exp index);
Tr_exp  Tr_fieldVar(Tr_exp base, int offset);

Tr_exp  Tr_binopExp(Tr_exp left , Tr_exp right ,A_oper oper );
Tr_exp  Tr_relopExp(Tr_exp left , Tr_exp right , A_oper oper) ;

Tr_exp Tr_ifExp( Tr_exp test , Tr_exp then , Tr_exp elses ) ;
Tr_exp Tr_recordExp( Tr_expList explist , int num) ;
Tr_exp Tr_arryExp(Tr_exp index , Tr_exp init) ;
Tr_exp Tr_seqExp(Tr_expList explist) ;
Tr_exp Tr_assignExp(Tr_exp exp1 , Tr_exp exp2) ;

Tr_exp Tr_whileExp(  Tr_exp test , Tr_exp body , bool isbreak) ;
Tr_exp Tr_forExp(Tr_access acc , Tr_exp hi , Tr_exp low , Tr_exp body) ;
Tr_exp Tr_breakExp() ;
Tr_exp Tr_letExp(Tr_expList declist , Tr_exp exp ) ;

Tr_exp Tr_callExp(Temp_label label ,Tr_level funleve , Tr_level  level ,Tr_expList explist ) ;
Tr_exp Tr_StaticLink(Tr_level now, Tr_level def);


Tr_exp Tr_varDec(Tr_access acc , Tr_exp init) ;
Tr_exp Tr_typeDec();
Tr_exp Tr_funDec(Tr_expList bodylist) ;

void Tr_procEntryExit(Tr_level level , Tr_exp body , Tr_accesslist formals) ;

F_fragList Tr_getResult() ;

#endif