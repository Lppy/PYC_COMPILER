#ifndef SENMANT_H_
#define SENMANT_H_
#include "types.h"
#include "translate.h"
#include "absyn.h"
struct expty { Tr_exp exp ; Ty_ty ty; };
expty expTy(Tr_exp exp , Ty_ty ty) ;
expty  transVar(S_table venv , S_table tenv , A_var var , Tr_level level) ;
expty  transExp(S_table venv , S_table tenv , A_exp exp , Tr_level level  ) ; //∑µªÿ÷µ÷–µƒtyty“ª∂® «◊ÓµÕ≤„µƒ¿‡–Õ
Tr_exp   transDec(S_table venv , S_table tenv , A_dec dec , Tr_level level ) ;
Ty_ty  transTy( S_table tenv , A_ty ty) ;
//ƒ⁄÷√±Í æ∑?ºÏ≤‚ int string ÷ª”√‘⁄œÚ÷µª∑æ≥ ‰»Î÷µµƒ ±∫Ú≤≈ºÏ≤‚  ¿‡–Õª∑æ≥‘⁄ tranTy÷–“—æ≠ºÏ≤‚
bool  innerIdentifiers( S_symbol sym);
#endif