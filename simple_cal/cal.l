%{
#include <stdlib.h>
#include "cal.tab.h"

int yylval;
%}

%%

"+"	{return ADD;}
"-"	{return SUB;}
"*"	{return MUL;}
"/"	{return DIV;}
"|"	{return ABS;}
[0-9]+	{yylval = atoi(yytext); return NUMBER;}
\n	{return EOL;}
[ \t]	{/* Ignore whitespace */}
.	{printf("Unrecognised character %c\n", *yytext);}
%%
