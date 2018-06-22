#include "assem.h"

#define FRAME_SIZE 4096

static FILE *fp;
static int data_max = 0;
static char now_func[20];
static char prev_lbl[20];
static bool is_prev_lbl = FALSE;

static void cmd(string msg){
    fprintf(fp, "\t%s\n", msg);
}

static void exp2asm(T_exp exp){
    if(is_prev_lbl){
        fprintf(fp, "%s: \n", prev_lbl);
        is_prev_lbl = FALSE;
    }
    switch(exp->kind){
        case T_BINOP:
            exp2asm(exp->u.BINOP.right);
            cmd("push ax");
            exp2asm(exp->u.BINOP.left);
            cmd("pop bx");
            switch(exp->u.BINOP.op){
                case T_plus:
                    cmd("add ax, bx");
                    break;
                case T_minus:
                    cmd("sub ax, bx");
                    break;
                case T_mul:
                    cmd("mul bx");
                    break;
                default:parse_error("not supported yet");assert(0);
            }
            break;
        case T_MEM:
            exp2asm(exp->u.MEM);
            cmd("mov bx, ax");
            cmd("mov ax, [bx]");
            break;
        case T_TEMP:{
            int tmp = exp->u.TEMP->num;
            if(tmp >= 102){
                fprintf(fp, "\tmov ax, T%d", tmp);
                data_max = data_max > tmp ? data_max : tmp;
            } else if(tmp == 101){
                cmd("mov ax, RET_VAL");
            } else if(tmp == 100){
                cmd("mov ax, sp");
            } else{
                assert(0);
            }
            break;
        }
//        case T_NAME:
//            fprintf(fp, "%s proc near\n", exp->u.NAME->name);
//            strcpy(now_func, exp->u.NAME->name);
//            break;
        case T_CONST:
            fprintf(fp, "\tmov ax, %d\n", exp->u.CONST);
            break;
        case T_CALL:{
            T_expList l = exp->u.CALL.args;
            int count = 0;
            fprintf(fp, "\tadd sp, %d\n", FRAME_SIZE);
            cmd("push bp");
            cmd("mov bp, sp");
            while(l){
                T_exp h = l->head;
                exp2asm(h);
//                cmd("push ax");
                fprintf(fp, "\tmov [bp-%d+%d], ax\n", FRAME_SIZE, count*4);
                count++;
                l = l->tail;
            }
            fprintf(fp, "\tcall %s\n", exp->u.CALL.fun->u.NAME->name);
//            fprintf(fp, "\tadd sp, %d\n", count);
            cmd("pop bp");
            break;
        }
        default:assert(0);
    }
}

static void stm2asm(T_stm stm){
    if(is_prev_lbl && stm->kind != T_LABEL && stm->kind != T_MOVE){
        fprintf(fp, "%s: \n", prev_lbl);
        is_prev_lbl = FALSE;
    }
    switch(stm->kind){//T_SEQ, T_LABEL, T_JUMP, T_CJUMP, T_MOVE, T_EXP, T_RET
        case T_LABEL:
            sprintf(prev_lbl, "%s", stm->u.LABEL->name);
            if(strcmp(prev_lbl, "main") == 0)
                strcpy(prev_lbl, "begin");
            is_prev_lbl = TRUE;
            break;
        case T_JUMP:
            fprintf(fp, "\tjmp %s\n", stm->u.JUMP.exp->u.NAME->name);
            break;
        case T_CJUMP:{//T_eq, T_ne, T_lt, T_gt, T_le, T_ge
            string t, f;
            char op[5];
            t = stm->u.CJUMP.trues->name;
            f = stm->u.CJUMP.falses->name;
            exp2asm(stm->u.CJUMP.right);
            cmd("push ax");
            exp2asm(stm->u.CJUMP.left);
            cmd("pop bx");
            cmd("test ax, bx");
            switch(stm->u.CJUMP.op){
                case T_eq:
                    strcpy(op, "je");
                    break;
                case T_ne:
                    strcpy(op, "jne");
                    break;
                case T_lt:
                    strcpy(op, "jb");
                    break;
                case T_gt:
                    strcpy(op, "ja");
                    break;
                case T_le:
                    strcpy(op, "jna");
                    break;
                case T_ge:
                    strcpy(op, "jnb");
                    break;
                default:assert(0);
            }
            fprintf(fp, "\t%s %s\n", op, t);
            fprintf(fp, "\tjmp %s\n", f);
            break;
        }
        case T_MOVE:{
            int tmp = stm->u.MOVE.dst->u.TEMP->num;
            if(stm->u.MOVE.dst->kind == T_TEMP && \
               tmp == 100 &&\
               is_prev_lbl && \
               stm->u.MOVE.src->kind == T_BINOP && \
               stm->u.MOVE.src->u.BINOP.right->kind == T_CONST && \
               stm->u.MOVE.src->u.BINOP.right->u.CONST < 0){
                fprintf(fp, "%s proc near\n", prev_lbl);
                is_prev_lbl = FALSE;
                strcpy(now_func, prev_lbl);
            } else if(is_prev_lbl){
                fprintf(fp, "%s: \n", prev_lbl);
                is_prev_lbl = FALSE;
            }
            exp2asm(stm->u.MOVE.src);
            if(stm->u.MOVE.dst->kind == T_TEMP){
                if(tmp >= 102){
                    fprintf(fp, "\tmov T%d, ax\n", tmp);
                    data_max = data_max > tmp ? data_max : tmp;
                } else if(tmp == 101){
                    cmd("mov RET_VAL, ax");
                } else if(tmp == 100){
                    cmd("mov sp, ax");
                } else{
                    assert(0);
                }
            } else if(stm->u.MOVE.dst->kind == T_MEM){
                cmd("push ax");
                exp2asm(stm->u.MOVE.dst);
                cmd("pop bx");
                cmd("mov [ax], bx");
            } else{
                parse_error("not supported yet"); assert(0);
            }
            break;
        }
        case T_EXP:
            exp2asm(stm->u.EXP);
            break;
        case T_RET:
            cmd("ret");
            fprintf(fp, "%s endp\n", now_func);
            break;
        default:
            assert(0);
    }
}

void assem(T_stmList list){
    fp = fopen("dosx86.asm", "w");
    for(list = list->tail; list; list = list->tail){
        stm2asm(list->head);
    }
}


