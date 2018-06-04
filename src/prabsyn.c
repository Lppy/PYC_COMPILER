#include "prabsyn.h"
#include <assert.h>

void pr_decList(FILE *out, A_decList v, int d) {
	indent(out, d);
	if(v) {
		fprintf(out, "decList(\n"); 
		pr_dec(out, v->head, d+1); fprintf(out, ",\n");
		pr_decList(out, v->tail, d+1);
		fprintf(out, ")");
	}
	else fprintf(out, "decList()"); 
}

static void indent(FILE *out, int d) {
	int i;
	for (i = 0; i <= d; i++) fprintf(out, " ");
}

static void pr_dec(FILE *out, A_dec v, int d) {
	indent(out, d);
	switch (v->kind) {
	case A_functionDec:
		fprintf(out, "functionDec(%s,\n", S_name(v->u.function.name)); 
		pr_fieldList(out, v->u.function.params, d+1); fprintf(out, "\n");
		pr_ty(out, v->u.function.result, d+1); fprintf(out, "\n");
		pr_exp(out, v->u.function.body, d+1); fprintf(out, ")");
		break;
	case A_varDec:
		fprintf(out, "varDec(\n");
		pr_ty(out, v->u.var.typ, d+1); fprintf(out, "\n"); 
		pr_efieldList(out, v->u.var.varList, d+1); fprintf(out, ",\n");
		indent(out, d+1); fprintf(out, "%s", v->u.var.escape ? "TRUE)" : "FALSE)");
		break;
	case A_structDec:
		fprintf(out, "structDec(%s,\n", S_name(v->u.structt.typ->u.name)); 
		pr_fieldList(out, v->u.structt.structure, d+1); fprintf(out, ")");
		break;
	default:
		assert(0); 
	} 
}

static void pr_ty(FILE *out, A_ty v, int d) {
	indent(out, d);
	switch (v->kind) {
	case A_nameTy:
		fprintf(out, "nameTy(%s)", S_name(v->u.name));
		break;
	case A_structTy:
		fprintf(out, "structTy(%s)", S_name(v->u.name));
		break;
	case A_arrayTy:
		fprintf(out, "arrayTy(%d,\n", v->u.array.length);
		pr_ty(out, v->u.array.tar, d+1); fprintf(out, ")");
		break;
	default:
		assert(0); 
	} 
}

static void pr_field(FILE *out, A_field v, int d) {
	indent(out, d);
	fprintf(out, "field(%s,\n", S_name(v->name));
	pr_ty(out, v->typ, d+1); fprintf(out, "\n");
	indent(out, d+1); fprintf(out, "%s", v->escape ? "TRUE)" : "FALSE)");
}

static void pr_fieldList(FILE *out, A_fieldList v, int d) {
	indent(out, d);
	if (v) {
		fprintf(out, "fieldList(\n");
		pr_field(out, v->head, d+1); fprintf(out, ",\n");
		pr_fieldList(out, v->tail, d+1); fprintf(out, ")");
	}
	else fprintf(out, "fieldList()");
}

static void pr_efield(FILE *out, A_efield v, int d) {
	indent(out, d);
	if (v) {
		fprintf(out, "efield(%s,\n", S_name(v->name));
		if(v->exp) {
			pr_exp(out, v->exp, d+1); fprintf(out, ")");
		}
		else{
			indent(out, d+1); fprintf(out, ")");
		}
	}
	else fprintf(out, "efield()");
}

static void pr_efieldList(FILE *out, A_efieldList v, int d) {
	indent(out, d);
	if (v) {
		fprintf(out, "efieldList(\n"); 
		pr_efield(out, v->head, d+1); fprintf(out, ",\n");
		pr_efieldList(out, v->tail, d+1); fprintf(out, ")");
	}
	else fprintf(out, "efieldList()");
}

void pr_exp(FILE *out, A_exp v, int d) {
	indent(out, d);
	switch (v->kind) {
	case A_varExp:
		fprintf(out, "varExp(\n"); 
		pr_var(out, v->u.var, d+1); fprintf(out, ")");
		break;
	case A_nilExp:
		fprintf(out, "nilExp()");
		break;
	case A_intExp:
		fprintf(out, "intExp(%d)", v->u.intt);
		break;
	case A_charExp:
		fprintf(out, "charExp(%c)", v->u.charr);
		break;
	case A_floatExp:
		fprintf(out, "floatExp(%lf)", v->u.floatt);
		break;
	case A_stringExp:
		fprintf(out, "stringExp(%s)", v->u.stringg);
		break;
	case A_callExp:
		fprintf(out, "callExp(%s,\n", S_name(v->u.call.func));
		pr_expList(out, v->u.call.args, d+1); fprintf(out, ")");
		break;
	case A_conExp:
		fprintf(out, "conExp(\n"); 
		pr_exp(out, v->u.con.left, d+1); fprintf(out, ",\n"); 
		pr_exp(out, v->u.con.mid, d+1); fprintf(out, ",\n"); 
		pr_exp(out, v->u.con.right, d+1); fprintf(out, ")");
		break;
	case A_opExp:
		fprintf(out, "opExp(\n");
		indent(out, d+1); pr_oper(out, v->u.op.oper); fprintf(out, ",\n");
		pr_exp(out, v->u.op.left, d+1); fprintf(out, ",\n"); 
		pr_exp(out, v->u.op.right, d+1); fprintf(out, ")");
		break;
	case A_unaryExp:
		fprintf(out, "unaryExp(\n");
		indent(out, d+1); pr_unoper(out, v->u.unary.unoper); fprintf(out, ",\n");
		pr_exp(out, v->u.unary.exp, d+1); fprintf(out, ")");
		break;
	case A_seqExp:
		fprintf(out, "seqExp(\n");
		pr_expList(out, v->u.seq, d+1); fprintf(out, ")");
		break;
	case A_assignExp:
		fprintf(out, "assignExp(\n");
		pr_var(out, v->u.assign.var, d+1); fprintf(out, ",\n");
		pr_exp(out, v->u.assign.exp, d+1); fprintf(out, ")");
		break;
	case A_ifExp:
		fprintf(out, "ifExp(\n");
		pr_exp(out, v->u.iff.test, d+1); fprintf(out, ",\n");
		pr_exp(out, v->u.iff.then, d+1);
		if (v->u.iff.elsee) { /* else is optional */
			fprintf(out, ",\n");
			pr_exp(out, v->u.iff.elsee, d+1);
		}
		fprintf(out, ")");
		break;
	case A_whileExp:
		fprintf(out, "whileExp(\n");
		pr_exp(out, v->u.whilee.test, d+1); fprintf(out, ",\n");
		pr_exp(out, v->u.whilee.body, d+1); fprintf(out, ")\n");
		break;
	case A_forExp:
		fprintf(out, "forExp(\n"); 
		pr_exp(out, v->u.forr.e1, d+1); fprintf(out, ",\n");
		pr_exp(out, v->u.forr.e2, d+1); fprintf(out, ",\n");
		pr_exp(out, v->u.forr.e3, d+1); fprintf(out, ",\n");
		pr_exp(out, v->u.forr.body, d+1); fprintf(out, ")");
		break;
	case A_breakExp:
		fprintf(out, "breakExp()");
		break;
	case A_continueExp:
		fprintf(out, "continueExp()");
		break;
	case A_letExp:
		fprintf(out, "letExp(\n");
		pr_decList(out, v->u.let.decs, d+1); fprintf(out, ",\n");
		pr_exp(out, v->u.let.body, d+1); fprintf(out, ")");
		break;
	case A_caseExp:
		fprintf(out, "caseExp(\n");
		pr_exp(out, v->u.casee.constant, d+1); fprintf(out, ",\n");
		pr_exp(out, v->u.casee.body, d+1); fprintf(out, ")");
		break;
	case A_switchExp:
		fprintf(out, "switchExp(\n");
		pr_exp(out, v->u.switchh.test, d+1); fprintf(out, ",\n");
		pr_expList(out, v->u.switchh.bodyList, d+1); fprintf(out, ")");
		break;
	case A_returnExp:
		fprintf(out, "returnExp(");
		if (v->u.returnn.res) { /* exp is optional */
			fprintf(out, "\n");
			pr_exp(out, v->u.returnn.res, d+1);
		}
		fprintf(out, ")");
		break;
	default:
		assert(0); 
	} 
}

static void pr_var(FILE *out, A_var v, int d) {
	indent(out, d);
	switch (v->kind) {
	case A_simpleVar:
		fprintf(out, "simpleVar(%s)", S_name(v->u.simple)); 
		break;
	case A_fieldVar:
		fprintf(out, "%s\n", "fieldVar(");
		pr_var(out, v->u.field.var, d+1); fprintf(out, ",\n"); 
		indent(out, d+1); fprintf(out, "%s)", S_name(v->u.field.sym));
		break;
	case A_subscriptVar:
		fprintf(out, "%s\n", "subscriptVar(");
		pr_var(out, v->u.subscript.var, d+1); fprintf(out, ",\n"); 
		pr_exp(out, v->u.subscript.exp, d+1); fprintf(out, ")");
		break;
	case A_addressVar:
		fprintf(out, "%s\n", "addressVar(");
		pr_var(out, v->u.address, d+1); fprintf(out, ")"); 
		break;
	default:
		assert(0); 
	} 
}

static char str_oper[][18] = {
	"+", "-", "*", "/", "%%", "<<", ">>", "&", "^", "|", "&&", "||",
	"==", "!=", "<", "<=", ">", ">="
};
 
static void pr_oper(FILE *out, A_oper d) {
	fprintf(out, "%s", str_oper[d]);
}

static char str_unoper[][4] = {
	"->", "&", "!", "~"
};

static void pr_unoper(FILE *out, A_unoper d) {
	fprintf(out, "%s", str_unoper[d]);
}

static void pr_expList(FILE *out, A_expList v, int d) {
	indent(out, d);
	if (v) {
		fprintf(out, "expList(\n"); 
		pr_exp(out, v->head, d+1); fprintf(out, ",\n");
		pr_expList(out, v->tail, d+1);
		fprintf(out, ")");
	}
	else fprintf(out, "expList()"); 
}








