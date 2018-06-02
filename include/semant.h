#ifndef SENMANT_H_
#define SENMANT_H_
#include "types.h"
#include "translate.h"
#include "absyn.h"
struct expty { Tr_exp exp ; Ty_ty ty; };
expty expTy(Tr_exp exp , Ty_ty ty) ;
expty  transVar(S_table venv , S_table tenv , A_var var , Tr_level level) ;
expty  transExp(S_table venv , S_table tenv , A_exp exp , Tr_level level  ) ; 
Tr_exp   transDec(S_table venv , S_table tenv , A_dec dec , Tr_level level ) ;
Ty_ty  transTy( S_table tenv , A_ty ty) ;

bool  innerIdentifiers( S_symbol sym);
#endif