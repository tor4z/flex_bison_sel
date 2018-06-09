%option noyywrap
%x IFILE

%{
#include <stdio.h>
#include <stdlib.h>


struct bufstack {
	struct bufstack *prev;
	YY_BUFFER_STATE bs;
	int lineno;
	char *filename;
	FILE *f;
} *curbs;

char *currfilename;
int newfile(char *filename);
int popfile(void);
%}

%%

^"#"[ \t]*include[ \t]*[\"<]	{
					BEGIN(IFILE);
				}

<IFILE>[^ \t\n\">]+		{

					char *fn = yytext;
					char c;
					while((c = input()) && c != '\n');

					yylineno++;
					if(!(newfile(fn)))
						yyterminate(); /* no such file */
					BEGIN(INITIAL);
				}

<IFILE>.|\n	{
			fprintf(stderr, "%4d bad include file\n", yylineno);
			yyterminate();
		}

<<EOF>>		{
			if(!popfile()) yyterminate();
		}

^.		{
			fprintf(yyout, "%4d %s", yylineno, yytext);
		}

^\n		{
			fprintf(yyout, "%4d %s", yylineno++, yytext);
		}

\n		{
			ECHO;
			yylineno++;
		}

.		{
			ECHO;
		}

%%

int main(int argc, char **argv)
{
	if(argc < 2) {
		fprintf(stderr, "need filename\n");
		return 1;
	}
	if(newfile(argv[1]))
		yylex();
}


int newfile(char *filename)
{
	FILE *f = fopen(filename, "r");
	struct bufstack *bs = (struct bufstack*)malloc(sizeof(struct bufstack));
	
	if(!f) {
		perror(filename);
		return 0;
	}
	if(!bs) {
		perror("malloc");
		exit(1);
	}
	
	if(curbs)
		curbs->lineno = yylineno;

	bs->prev = curbs;
	bs->bs = yy_create_buffer(f, YY_BUF_SIZE);
	bs->f = f;
	bs->filename = filename;

	yy_switch_to_buffer(bs->bs);
	curbs = bs;
	yylineno = 1;
	currfilename = filename;
	return 1;
}


int popfile(void)
{
	struct bufstack *bs = curbs;
	struct bufstack *prevbs;
	
	if(!bs) return 0;

	fclose(bs->f);
	yy_delete_buffer(bs->bs);

	prevbs = bs->prev;
	free(bs);

	if(!prevbs)
		return 0;
	
	yy_switch_to_buffer(prevbs->bs);
	curbs = prevbs;
	yylineno = curbs->lineno;
	currfilename = curbs->filename;
	return 1;
}
