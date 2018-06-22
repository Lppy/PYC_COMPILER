#include "printtree.h"

static void pr_tree_exp(FILE *out, T_exp exp, int d);

static void indent(FILE *out, int d) {
	int i;
	for (i = 0; i <= d; i++) fprintf(out, " ");
}

static char bin_oper[][12] = {
	"PLUS", "MINUS", "TIMES", "DIVIDE", "MOD",
	"AND", "OR", "LSHIFT", "RSHIFT", "ARSHIFT", "XOR"
};

static char rel_oper[][12] = {
	"EQ", "NE", "LT", "GT", "LE", "GE", "ULT", "ULE", "UGT", "UGE"
};
 
static void pr_stm(FILE *out, T_stm stm, int d)
{
	switch(stm->kind) {
	case T_SEQ:
		indent(out, d); fprintf(out, "\"name\":\"SEQ\",\"children\":[{"); 
		pr_stm(out, stm->u.SEQ.left, d+1);  fprintf(out, "},{"); 
		pr_stm(out, stm->u.SEQ.right, d+1); fprintf(out, "}]");
		break;
	case T_LABEL:
		indent(out, d); fprintf(out, "\"name\":\"LABEL %s\"", S_name(stm->u.LABEL));
		break;
	case T_JUMP:
		indent(out, d); fprintf(out, "\"name\":\"JUMP\",\"children\":[{"); 
		pr_tree_exp(out, stm->u.JUMP.exp,d+1); fprintf(out, "}]");
		break;
	case T_CJUMP:
		indent(out, d); fprintf(out, "\"name\":\"CJUMP %s\",\"children\":[{", rel_oper[stm->u.CJUMP.op]);
		pr_tree_exp(out, stm->u.CJUMP.left, d+1); fprintf(out, "},{"); 
		pr_tree_exp(out, stm->u.CJUMP.right, d+1); fprintf(out, "},{");
		indent(out, d+1); 
		fprintf(out, "\"name\":\"TRUES %s\"},{", S_name(stm->u.CJUMP.trues));
		fprintf(out, "\"name\":\"FALSES %s\"}]", S_name(stm->u.CJUMP.falses));
		break;
	case T_MOVE:
		indent(out, d); fprintf(out, "\"name\":\"MOVE\",\"children\":[{"); 
		pr_tree_exp(out, stm->u.MOVE.dst, d+1); fprintf(out, "},{");
		pr_tree_exp(out, stm->u.MOVE.src, d+1); fprintf(out, "}]");
		break;
	case T_EXP:
		indent(out, d); fprintf(out, "\"name\":\"EXP\",\"children\":[{"); 
		pr_tree_exp(out, stm->u.EXP, d+1); fprintf(out, "}]");
		break;
	case T_RET:
		indent(out, d); fprintf(out, "\"name\":\"RETURN\""); 
		break;
	}
}

static void pr_tree_exp(FILE *out, T_exp exp, int d)
{
	switch(exp->kind) {
	case T_BINOP:
		indent(out, d); fprintf(out, "\"name\":\"BINOP %s\",\"children\":[{", bin_oper[exp->u.BINOP.op]); 
		pr_tree_exp(out, exp->u.BINOP.left, d+1); fprintf(out, "},{"); 
		pr_tree_exp(out, exp->u.BINOP.right, d+1); fprintf(out, "}]");
		break;
	case T_MEM:
		indent(out, d); fprintf(out, "\"name\":\"MEM\",\"children\":[{");
		pr_tree_exp(out, exp->u.MEM, d+1); fprintf(out, "}]");
		break;
	case T_TEMP:
		indent(out, d); fprintf(out, "\"name\":\"TEMP t%s\"", Temp_look(Temp_name(), exp->u.TEMP));
		break;
	case T_ESEQ:
		indent(out, d); fprintf(out, "\"name\":\"ESEQ\",\"children\":[{"); 
		pr_stm(out, exp->u.ESEQ.stm, d+1); fprintf(out, "},{");
		pr_tree_exp(out, exp->u.ESEQ.exp, d+1); fprintf(out, "}]");
		break;
	case T_NAME:
		indent(out, d); fprintf(out, "\"name\":\"NAME %s\"", S_name(exp->u.NAME));
		break;
	case T_CONST:
		indent(out, d); fprintf(out, "\"name\":\"CONST %d\"", exp->u.CONST);
		break;
	case T_CALL:
		{
			T_expList args = exp->u.CALL.args;
			indent(out, d); fprintf(out, "\"name\":\"CALL\",\"children\":[{"); 
			pr_tree_exp(out, exp->u.CALL.fun, d+1);
			for(; args; args=args->tail)
			{
				fprintf(out, "},{"); 
				pr_tree_exp(out, args->head, d+2);
			}
			fprintf(out, "}]");
			break;
		}
	}
}

void printProg(FILE *out, T_stm stm) 
{
	if(stm) {
		fprintf(out, "{\n");
		pr_stm(out, stm, 0); 
		fprintf(out, "\n}");
	}
}

void printList(FILE *out, T_stmList stm)
{
	if(stm == NULL)
		return;
	fprintf(out, "{\"name\":\"root\",\n\"children\":[\n");
	while(stm->tail != NULL){
		printProg(out, stm->head);
		fprintf(out, ",\n");
		stm = stm->tail;
	}
	printProg(out, stm->head);
	fprintf(out, "\n]}");

}
