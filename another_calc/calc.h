extern int yylineno;
void yyerror(const char *fmt, ...);


struct ast {
	char nodetype;
	struct ast *l;
	struct ast *r;
};


struct numval {
	char nodetype;
	double number;
};


struct ast *newast(char nodeType, struct ast *l, struct ast *r);
struct ast *newnum(double d);


double eval(struct ast *a);
void treefree(struct ast *a);
