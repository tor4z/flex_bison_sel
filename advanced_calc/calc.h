#ifndef _CALC_H
#define _CALC_H

extern int yylineno;
void yyerror(const char *fmt, ...);

struct symlist {
	struct symbol *sym;
	struct symlist *next;
};


struct symbol {
	char *name;
	double value;
	struct ast *func;
	struct symlist *syms;	/* List of dummy args */
};

#define NHASH 9997
struct symbol symtab[NHASH];

struct symbol *lookup(char *name);
struct symlist *newsymlist(struct symbol *sym, struct symlist *next);
void symlistfree(struct symlist *sl);

/* node type
 * + - * / | 
 * 0-7 comparison ops, bit coded 04 equal, 02 less, 01 greater
 * M nuary minus
 * L expression or statement list
 * I IF statement
 * W WHILE statement
 * N symbol ref
 * = assignment
 * S list of symbols
 * F build in function call
 * C user function call
 * */

enum bifs {
	B_sqrt = 1,
	B_exp,
	B_log,
	B_print
};

struct ast {
	int nodetype;
	struct ast *l;
	struct ast *r;
};


struct fncall {
	int nodetype;
	struct ast *l;
	enum bifs functype;
};


struct ufncall {
	int nodetype;
	struct ast *l;		/* List of arguments */
	struct symbol *s;
};


struct flow {
	int nodetype;
	struct ast *cond;
	struct ast *tl;		/* then branch or do list */
	struct ast *el;		/* optional else branch */
};


struct numval {
	int nodetype;
	double number;
};

struct symref {
	int nodetype;
	struct symbol *s;
};


struct symasgn {
	int nodetype;
	struct symbol *s;
	struct ast *v;		/* value */
};


/* build an AST */
struct ast *newast(int nodetype, struct ast *l, struct ast *r);
struct ast *newcmp(int cmptype, struct ast *l, struct ast *r);
struct ast *newfunc(int functype, struct ast *l);
struct ast *newcall(struct symbol *s, struct ast *l);
struct ast *newref(struct symbol *s);
struct ast *newnum(double d);
struct ast *newasgn(struct symbol *s, struct ast *v);
struct ast *newflow(int nodetype, struct ast *cond, struct ast *tl, struct ast *el);

/* define a function */
void dodef(struct symbol *name, struct symlist *syms, struct ast *stmts);

/* evaluate a AST */
double eval(struct ast *a);

/* delete and free a AST */
void treefree(struct ast *a);
void shell();

#endif // _CALC_H
