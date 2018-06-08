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
"("	{return OP;}
")"	{return CP;}
[0-9]+	{yylval = atoi(yytext); return NUMBER;}
\n	{return EOL;}
[ \t]	{/* Ignore whitespace */}
"//".*	{/* Comment, just ignore it! */}
.	{printf("Unrecognised character %c\n", *yytext);}
%%
