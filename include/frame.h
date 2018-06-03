#ifndef FRAME_H_
#define FRAME_H_

#include "util.h"
#include "temp.h"
#include "tree.h"

typedef struct F_frame_ *F_frame;
typedef struct F_access_ *F_access;
typedef struct F_accesslist_ *F_accesslist;
typedef struct F_frag_ *F_frag;
typedef struct F_fragList_ *F_fragList;

struct F_frame_ { 
    Temp_label name; 
    int framesize;
    F_accesslist formals; 
    F_accesslist locals; 
};

struct F_access_ {
    enum { inFrame, inReg } kind;
    union{
        int offset;
        Temp_temp reg;
    } u;
};

struct F_accesslist_ { F_access head; F_accesslist tail; };

struct F_frag_ {
    enum { F_stringFrag, F_procFrag } kind ;
    union {
        struct { Temp_label label; string str; } stringg;
        struct { T_stm body; F_frame frame; } proc;
    } u;
};

struct F_fragList_{  F_frag head; F_fragList tail; };

F_frag F_StringFrag(Temp_label label, string str);
F_frag F_ProcFrag (T_stm stm, F_frame frame);
F_fragList F_FragList(F_frag frag, F_fragList tail);

Temp_temp F_FP(void);   
Temp_temp F_RV(void);  
Temp_label F_RA(void);//return address

F_frame F_newframe(Temp_label name, U_boolList formals);
F_accesslist F_Accesslist(F_access head, F_accesslist tail);

Temp_label F_name(F_frame f);
F_accesslist F_formals(F_frame f);
F_access F_allocLoacl(F_frame f, bool escape);

static F_access InFrame(int offset);
static F_access InReg(Temp_temp reg);

T_exp F_Exp(F_access acc, T_exp framePtr);
T_exp F_AddressExp(F_access acc, T_exp framePtr);
T_exp F_externalCall(string s, T_expList explist);

T_stm F_procEntryExit1(F_frame frame, T_stm stm);

#endif