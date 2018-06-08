%{
#include <stdio.h>  
%}

%token NUMBER
%token ADD SUB MUL DIV ABS
%token OP CP
%token COMM
%token EOL

%%

calclist: /* nothing */
        | calclist exp EOL 	{printf("=%d\n", $2);}
	| calclist COMM EOL		{}
	;

exp : factor			{$$ = $1;}
	| exp ADD factor	{$$ = $1 + $3;}
	| exp SUB factor	{$$ = $1 - $3;}
	;

factor: term			{$$ = $1;}
	| factor MUL term	{$$ = $1 * $3;}
	| factor DIV term	{$$ = $1 / $3;}
	;
      
term: NUMBER			{$$ = $1;}
    	| ABS term		{$$ = $2 > 0 ? $2 : -$2;}
    	| OP exp CP		{$$ = $2;}
	;

%%

int main()
{
	yyparse();
	return 0;
}

void yyerror(char const *msg)
{
	fprintf(stderr, "Calc error: %s\n", msg);
}
