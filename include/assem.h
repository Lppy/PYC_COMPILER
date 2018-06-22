#include "tree.h"
#include "util.h"
#include "error.h"

static void cmd(string msg);
static void exp2asm(T_exp exp);
static void stm2asm(T_stm stm);
void assem(T_stmList list);