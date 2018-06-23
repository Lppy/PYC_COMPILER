#include "assem.h"

#define FRAME_SIZE 128

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
            cmd("mov si, ax");
            cmd("mov ax, ss:[si]");
            break;
        case T_TEMP:{
            int tmp = exp->u.TEMP->num;
            if(tmp >= 102){
//                fprintf(fp, "\tpop T%d\n", tmp);
                fprintf(fp, "\tmov ax, T%d\n", tmp);
                data_max = data_max > tmp ? data_max : tmp;
            } else if(tmp == 101){
                cmd("mov ax, RET_VAL");
            } else if(tmp == 100){
                cmd("mov ax, bp");
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
            int count = 0, i;
            for(i=102; i <= data_max; i++)
                fprintf(fp, "\tpush T%d\n", i);
            fprintf(fp, "\tsub sp, %d\n", FRAME_SIZE);
            while(l){
                T_exp h = l->head;
                exp2asm(h);
//                cmd("push ax");
                cmd("mov si, sp");
                fprintf(fp, "\tmov ss:[si+%d], ax\n", count*4);
                count++;
                l = l->tail;
            }
            fprintf(fp, "\tcall %s\n", exp->u.CALL.fun->u.NAME->name);
//            fprintf(fp, "\tadd sp, %d\n", count);
            fprintf(fp, "\tadd sp, %d\n", FRAME_SIZE);
            for(i=data_max; i >= 102; i--)
                fprintf(fp, "\tpop T%d\n", i);
            cmd("mov ax, RET_VAL");
            break;
        }
        default:assert(0);
    }
}

static void stm2asm(T_stm stm){
    if(is_prev_lbl && stm->kind != T_LABEL){
        fprintf(fp, "%s: \n", prev_lbl);
        is_prev_lbl = FALSE;
    }
    switch(stm->kind){//T_SEQ, T_LABEL, T_JUMP, T_CJUMP, T_MOVE, T_EXP, T_RET
        case T_LABEL:
            if(is_prev_lbl && strcmp(stm->u.LABEL->name, "func") == 0){
                fprintf(fp, "%s proc near\n\t"
                            "push bp\n\t"
                            "mov bp, sp\n"
//                            "add bp, 2\n"
                            , prev_lbl);
                is_prev_lbl = FALSE;
                strcpy(now_func, prev_lbl);
            } else{
                sprintf(prev_lbl, "%s", stm->u.LABEL->name);
                is_prev_lbl = TRUE;
            }
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
            cmd("cmp ax, bx");
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
//            if(stm->u.MOVE.dst->kind == T_TEMP && \
//               tmp == 100 &&\
//               is_prev_lbl && \
//               stm->u.MOVE.src->kind == T_BINOP && \
//               stm->u.MOVE.src->u.BINOP.right->kind == T_CONST && \
//               stm->u.MOVE.src->u.BINOP.right->u.CONST < 0){
//                fprintf(fp, "%s proc near\n\tmov FP, sp\n", prev_lbl);
//                is_prev_lbl = FALSE;
//                strcpy(now_func, prev_lbl);
//            } else if(is_prev_lbl){
//                fprintf(fp, "%s: \n", prev_lbl);
//                is_prev_lbl = FALSE;
//            }
            exp2asm(stm->u.MOVE.src);
            if(stm->u.MOVE.dst->kind == T_TEMP){
                if(tmp >= 102){
                    fprintf(fp, "\tmov T%d, ax\n", tmp);
//                    fprintf(fp, "\tpush T%d\n", tmp);
                    data_max = data_max > tmp ? data_max : tmp;
                } else if(tmp == 101){
                    cmd("mov RET_VAL, ax");
                } else if(tmp == 100){
//                    cmd("mov sp, ax");
                    assert(0);
                } else{
                    assert(0);
                }
            } else if(stm->u.MOVE.dst->kind == T_MEM){
                cmd("push ax");
                exp2asm(stm->u.MOVE.dst->u.MEM);
                cmd("pop bx");
                cmd("mov si, ax");
                cmd("mov ss:[si], bx");
            } else{
                parse_error("not supported yet"); assert(0);
            }
            break;
        }
        case T_EXP:
            exp2asm(stm->u.EXP);
            break;
        case T_RET:
            cmd("mov sp, bp");
            cmd("pop bp");
            cmd("ret");
            fprintf(fp, "%s endp\n\n", now_func);
            break;
        default:
            assert(0);
    }
}

void assem(T_stmList list){
    FILE *target = fopen("dosx86.asm", "w");
    char buf[100];
    
    fp = fopen("mid", "w");
    fprintf(fp,
            ".code\n"
            "begin proc near\n\t"
            "mov ax, @data\n\t"
            "mov ds, ax\n\t"
            "sub sp, %d\n\t"
            "call main\n\t"
            "mov ax, RET_VAL\n\t"
            "add sp, %d\n\n\t"
            "mov bl, 10\n\t"
            "mov cx, 10\n\t"
            "mov bh, 0\n"
            "lo1:\n\t"
            "div bl\n\t"
            "mov dl, ah\n\t"
            "add dl, 30h\n\t"
            "push dx\n\t"
            "inc bh\n\t"
            "and ax, 00ffh\n\t"
            "loopnz lo1\n\t"
            "mov cl, bh\n\t"
            "mov ah, 02h\n"
            "lo2:\n\t"
            "pop dx\n\t"
            "int 21h\n\t"
            "loop lo2\n\t"
            "mov ah, 4ch\n\t"
            "int 21h\n"
            "begin endp\n\n"
            , FRAME_SIZE, FRAME_SIZE);
    
    for(list = list->tail; list; list = list->tail){
        stm2asm(list->head);
    }
    fprintf(fp, "end begin\n\n");
    fclose(fp);
    
    fprintf(target, ".model small\n\n.data\n");
    fprintf(target, "RET_VAL dw 0\n"
                    "FP dw 0\n");
    while(data_max >= 102){
        fprintf(target, "T%d dw 0\n", data_max--);
    }
    fprintf(target, "\n.stack %d\n\n", FRAME_SIZE*64);
    fp = fopen("mid", "r");
    while(fgets(buf, 100, fp)){
        fputs(buf, target);
    }
}
//if(strcmp(prev_lbl, "begin") == 0){
//    fprintf(fp, "\tmov ax, @data\n\tmov ds, ax\n");
//}

