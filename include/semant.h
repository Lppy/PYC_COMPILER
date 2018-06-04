#ifndef SENMANT_H_
#define SENMANT_H_

#include "types.h"
#include "translate.h"
#include "absyn.h"
#include "frame.h"

struct expty { Tr_exp exp; Ty_ty ty; };
struct expty expTy(Tr_exp exp, Ty_ty ty);
struct expty transVar(S_table venv, S_table tenv, A_var var, Tr_level level);
struct expty transExp(S_table venv, S_table tenv, A_exp exp, Tr_level level); 
Tr_exp transDec(S_table venv, S_table tenv, A_dec dec, Tr_level level);
Ty_ty transTy(S_table tenv, A_ty ty);

T_stm transDecList(A_decList prog);

bool innerIdentifiers(S_symbol sym);

#endif
