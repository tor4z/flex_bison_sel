#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include "calc.h"
#include "calc.tab.h"


static double callbuildin(struct fncall *f);
static double calluser(struct ufncall *f);


static unsigned int symhash(char *sym)
{
	unsigned int hash = 0;
	unsigned c;
	while(c = *sym++)
		hash = (hash * 9)^c;

	return hash;
}


struct symbol *lookup(char *sym)
{
	struct symbol *sp = &symtab[symhash(sym) % NHASH];
	int scount = NHASH;

	while(--scount >= 0) {
		if(sp->name && !strcmp(sp->name, sym))
			return sp;

		if(!sp->name) {
			sp->name = strdup(sym);
			sp->value = 0;
			sp->func = NULL;
			sp->syms = NULL;
			return sp;
		}

		if(++sp >= symtab + NHASH)
			sp = symtab;
	}
	
	yyerror("symtab overflow\n");
	abort();
}


struct ast *newast(int nodetype, struct ast *l, struct ast *r)
{
	struct ast *a = (struct ast*)malloc(sizeof(struct ast));

	if(!a) {
		yyerror("out of space\n");
		exit(1);
	}

	a->nodetype = nodetype;
	a->l = l;
	a->r = r;
	return a;
}


struct ast *newnum(double d)
{
	struct numval *n = (struct numval*)malloc(sizeof(struct numval));
	if(!n) {
		yyerror("out of space\n");
		exit(1);
	}

	n->nodetype = 'K';
	n->number = d;
	return (struct ast*)n;
}


struct ast *newcmp(int cmptype, struct ast *l, struct ast *r)
{
	struct ast *a = (struct ast*)malloc(sizeof(struct ast));
	if(!a) {
		yyerror("out of space\n");
		exit(1);
	}

	a->nodetype = '0' + cmptype;
	a->l = l;
	a->r = r;
	return a;
}


struct ast *newfunc(int functype, struct ast *l)
{
	struct fncall *f = (struct fncall*)malloc(sizeof(struct fncall));
	if(!f) {
		yyerror("out of space\n");
		exit(1);
	}

	f->nodetype = 'F';
	f->l = l;
	f->functype = functype;
	return (struct ast*)f;
}


struct ast *newcall(struct symbol *s, struct ast *l)
{
	struct ufncall *f = (struct ufncall*)malloc(sizeof(struct ufncall));
	if(!f) {
		yyerror("out of space\n");
		exit(1);
	}

	f->nodetype = 'C';
	f->l = l;
	f->s = s;
	return (struct ast*)f;
}


struct ast *newref(struct symbol *s)
{
	struct symref *r = (struct symref*)malloc(sizeof(struct symref));
	if(!r) {
		yyerror("out of space\n");
		exit(1);
	}

	r->nodetype = 'N';
	r->s = s;
	return (struct ast*)r;
}


struct ast *newasgn(struct symbol *s, struct ast *v)
{
	struct symasgn *a = (struct symasgn*)malloc(sizeof(struct symasgn));
	if(!a) {
		yyerror("out of space\n");
		exit(1);
	}

	a->nodetype = '=';
	a->s = s;
	a->v = v;
	return (struct ast*)a;
}


struct ast *newflow(int nodetype, struct ast *cond, struct ast *tl, struct ast *el)
{
	struct flow *f = (struct flow*)malloc(sizeof(struct flow));
	if(!f) {
		yyerror("out of space\n");
		exit(1);
	}

	f->nodetype = nodetype;
	f->cond = cond;
	f->tl = tl;
	f->el = el;
	return (struct ast*)f;
}


void treefree(struct ast *a)
{
	switch(a->nodetype) {
		case '+':
		case '-':
		case '*':
		case '/':
		case '1': case '2': case '3': case '4': case '5': case '6':
		case 'L':
			treefree(a->r);
		case '|':
		case 'M': case 'C': case 'F':
			treefree(a->l);
		case 'K': case 'N':
			break;
		case '=':
			free(((struct symasgn*)a)->v);
			break;
		case 'I': case 'W':
			free(((struct flow*)a)->cond);
			if(((struct flow*)a)->tl) treefree(((struct flow*)a)->tl);
			if(((struct flow*)a)->el) treefree(((struct flow*)a)->el);
			break;
		default:
			printf("internal error: free bed node %c\n", a->nodetype);
	}
	free(a);
}


struct symlist *newsymlist(struct symbol *sym, struct symlist *next)
{
	struct symlist *sl = (struct symlist*)malloc(sizeof(struct symlist));
	if(!sl) {
		yyerror("out of space\n");
		exit(1);
	}

	sl->sym = sym;
	sl->next = next;
	return sl;
}


void symlistfree(struct symlist *sl)
{
	struct symlist *nsl;

	while(sl) {
		nsl = sl->next;
		free(sl);
		sl = nsl;
	}
}


double eval(struct ast *a)
{
	double v;

	if(!a) {
		yyerror("internal error, null eval\n");
		return 0.0;
	}
	switch(a->nodetype) {
		case 'K':
			v = ((struct numval*)a)->number;
			break;
		case 'N':
			v = ((struct symref *)a)->s->value;
			break;
		case '=':
			v = ((struct symasgn *)a)->s->value =
				eval(((struct symasgn *)a)->v);
			break;
		case '+':
			v = eval(a->l) + eval(a->r);
			break;
		case '-':
			v = eval(a->l) - eval(a->r);
			break;
		case '*':
			v = eval(a->l) * eval(a->r);
			break;
		case '/':
			v = eval(a->l) / eval(a->r);
			break;
		case '|':
			v = fabs(eval(a->l));
			break;
		case 'M':
			v = -eval(a->l);
			break;
		case '1':
			v = (eval(a->l) > eval(a->r)) ? 1 : 0;
			break;
		case '2':
			v = (eval(a->l) < eval(a->r)) ? 1 : 0;
			break;
		case '3':
			v = (eval(a->l) != eval(a->r)) ? 1 : 0;
			break;
		case '4':
			v = (eval(a->l) == eval(a->r)) ? 1 : 0;
			break;
		case '5':
			v = (eval(a->l) >= eval(a->r)) ? 1 : 0;
			break;
		case '6':
			v = (eval(a->l) <= eval(a->r)) ? 1 : 0;
			break;
		case 'I':
			if(eval(((struct flow*)a)->cond) != 0) {
				if(((struct flow*)a)->tl)
					v = eval(((struct flow*)a)->tl);
				else
					v = 0.0;
			} else {
				if(((struct flow*)a)->el)
					v = eval(((struct flow*)a)->el);
				else
					v = 0.0;
			}
			break;
		case 'W':
			v = 0.0;
			if(((struct flow*)a)->tl) {
				while(eval(((struct flow*)a)->cond) != 0) {
					v = eval(((struct flow*)a)->tl);
				}
			}
			break;
		case 'L':
			eval(a->l);
			v = eval(a->r);
			break;
		case 'F':
			v = callbuildin((struct fncall*)a);
			break;
		case 'C':
			v = calluser((struct ufncall*)a);
			break;
		default:
			printf("internal error: bad node %c\n", a->nodetype);

	}
	return v;
}


static double callbuildin(struct fncall *f)
{
	enum bifs functype = f->functype;
	double v = eval(f->l);

	switch(functype) {
		case B_sqrt:
			return sqrt(v);
		case B_exp:
			return exp(v);
		case B_log:
			return log(v);
		case B_print:
			printf("=%4.4g\n", v);
			return v;
		default:
			yyerror("Unknow build-in function %d\n", functype);
			return 0.0;
	}
}


void dodef(struct symbol *name, struct symlist *syms, struct ast *func)
{
	if(name->syms) symlistfree(name->syms);
	if(name->func) treefree(name->func);

	name->syms = syms;
	name->func = func;
}


static double calluser(struct ufncall *f)
{
	struct symbol *fn = f->s;
	struct symlist *sl;
	struct ast *args = f->l;
	double *oldval, *newval;
	double v;
	int nargs;
	int i;

	if(!fn->func) {
		yyerror("call to undefine function %s\n", fn->name);
		return 0.0;
	}
	
	sl = fn->syms;
	for(nargs = 0; sl; sl->next)
		nargs++;
	
	oldval = (double *)malloc(nargs * sizeof(double));
	newval = (double *)malloc(nargs * sizeof(double));

	if(!oldval || !newval) {
		yyerror("out of space\n");
		exit(1);
	}

	for(i = 0; i < nargs; i++) {
		if(!args) {
			yyerror("too few args in call function %s\n", fn->name);
			free(oldval);
			free(newval);
			return 0.0;
		}

		if(args->nodetype == 'L') {
			newval[i] = eval(args->l);
			args = args->r;
		} else {
			newval[i] = eval(args);
			args = NULL;
		}
	}

	sl = fn->syms;
	for(i = 0; i < nargs; i++) {
		struct symbol *s = sl->sym;

		oldval[i] = s->value;
		s->value = newval[i];
		sl = sl->next;
	}

	free(newval);

	v = eval(fn->func);

	sl = fn->syms;
	for(i = 0; i < nargs; i++) {
		struct symbol *s = sl->sym;

		s->value = oldval[i];
		sl = sl->next;
	}
	
	free(oldval);
	return v;
}

void yyerror(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);

	fprintf(stderr, "%d error: ", yylineno);
	vfprintf(stderr, fmt, ap);
}


void shell()
{
	printf("> ");
}

int main()
{
	shell();
	return yyparse();
}
