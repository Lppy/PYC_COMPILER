#include "types.h"
#include "absyn.h"
#include <assert.h>

typedef void *Tr_exp;

struct expty {Tr_exp exp; Ty_ty ty;};

struct expty expTy(Tr_exp exp, Ty_ty ty){
	struct expty e;
	e.exp=exp;e.ty=ty;return e;
}

struct expty transExp(S_table venv, S_table tenv, A_exp a){
	if(!exp)assert(0);
	switch(exp->kind)
	
}