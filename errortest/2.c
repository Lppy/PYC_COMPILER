// syntax error: missing semicolon in definition of struct s.b

struct s{
	int a;
	int *b
	int **c;
};
int main(){
	struct s sb;
	sb.a = 1;
	sb.b = &sb.a;
	sb.c = &(sb.c[0][sb.a*2]);
	return s;
}#
