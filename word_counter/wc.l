%{
#include <stdio.h>
#include <string.h>

int chars =  0;
int lines =  0;
int words =  0;

int tot_chars = 0;
int tot_lines = 0;
int tot_words = 0;
%}


%%

[a-zA-Z]+	{++words; chars += strlen(yytext);}
\n		{++lines; ++chars;}
.		{++chars;}

%%

int main(int argc, char **argv)
{
	if(argc < 2) {
		yylex();
		printf("words: %d\nlines: %d\nchars: %d\n", words, lines, chars);
		return 0;
	}

	int i;
	FILE *f;
	for(i = 1; i< argc; i++){
		if(!(f = fopen(argv[i], "r"))) {
			fprintf(stderr, "Can not open %s\n", argv[1]);
			return 1;
		}
		yyrestart(f);
		yylex();
		fclose(f);

		tot_chars += chars;		
		tot_words += words;		
		tot_lines += lines;
		
		printf("%s\n", argv[i]);
		printf("words: %d\nlines: %d\nchars: %d\n", words, lines, chars);
		printf("---------------\n");
	}
	printf("Total words: %d\n", tot_words);
	printf("Total lines: %d\n", tot_lines);
	printf("Total chars: %d\n", tot_chars);
	return 0;
}
