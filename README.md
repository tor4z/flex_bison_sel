# flex & bison SEL

## Flex quick notes

1. a(bc|de) matches abc or ade. 

2. 0/1 matches 0 in the string 01 but would not match anything in the string 0 or 02. 
The material matched by the pattern following the slash is not “consumed” and remains
to be turned into subsequent tokens. 

3. How Flex Handles Ambiguous Patterns
* Match the longest possible string every time the scanner matches input.
* In the case of a tie, use the pattern that appears first in the program.

4. the three levels of input management are:
* Setting yyin to read the desired file(s)
* Creating and using `YY_BUFFER_STATE` input buffers
* Redefining `YY_INPUT`

5. Harking back to the earliest versions of lex, unless you tell it
otherwise, flex acts as though there is a default rule at the end of the scanner that copies
otherwise unmatched input to yyout.
```
.	ECHO;

#define ECHO fwrite( yytext, yyleng, 1, yyout )
```
`%option nodefault` at the top of the scanner to tell it not to add a default rule and rather
to report an error if the input rules don’t cover all possible input. 

6. Flex output [See](https://stackoverflow.com/questions/16995864/how-to-redirect-yyout-to-char-buffer-in-gnu-flex)

7. Hack due to limited lookahead
```
/* flex will match the longest */
ON 			{ return ON; }
ON[ \t\n]+DUPLICATE 	{ return ONDUPLICATE; } 

EXISTS	 		{ yylval.subtok = 0; return EXISTS; }
NOT[ \t\n]+EXISTS	{ yylval.subtok = 1; return EXISTS; }

```
Also note that the phrases NOT EXISTS and ON DUPLICATE are recognized as single tokens;
this is to avoid shift/reduce conflicts in the parser because of other contexts where
NOT and ON can appear. 

8. Hack for BETWEEN ... AND ... return special AND token if BETWEEN seen
```
<BTWMODE>AND	{ BEGIN INITIAL; return AND; }
AND		{ return ANDOP; }
```

9. State management
```
/* store old state and restore */
"/*" 		{ oldstate = YY_START; BEGIN COMMENT; }
<COMMENT>"*/" 	{ BEGIN oldstate; }
<COMMENT>.|\n
```

10. The default input behavior of a flex scanner is approximately this:
```
YY_BUFFER_STATE bp;
extern FILE* yyin;
/* ... whatever the program does before the first call to the scanner */

if(!yyin) yyin = stdin;  /* default input is stdin */
bp = yy_create_buffer(yyin,YY_BUF_SIZE );

/* YY_BUF_SIZE defined by flex, typically 16K */

yy_switch_to_buffer(bp); /* tell it to use the buffer we just made */
yylex(); 		 /* or yyparse() or whatever calls the scanner*/

```
11. `yy_buffer_state` Struct Reference
Since `yy_buf_pos` field can remenber the position of the buffer,
So when we use `yy_switch_to_buffer` to switch old buffer back, 
we can continue to parse from the old position **NOT** from the begin of the file.
```c
FILE * 		yy_input_file
char * 		yy_ch_buf	/* input buffer */
char * 		yy_buf_pos 	/* current position in input buffer */
yy_size_t 	yy_buf_size	/* Size of input buffer in bytes, not including room for EOB */
int 		yy_n_chars
int 		yy_is_our_buffer
int 		yy_is_interactive
int 		yy_at_bol
int 		yy_fill_buffer
int 		yy_buffer_status

/* http://www.ks.uiuc.edu/Research/vmd/doxygen/AtomLexer_8C-source.html */
```

## Bison quick notes

1. Advanced yylval: %union [More](https://www.tldp.org/HOWTO/Lex-YACC-HOWTO-6.html)

2. Shift/Reduce [See](https://www.epaperpress.com/lexandyacc/thy.html)

3. RPN Style

```
expr: NAME 		{ emit("NAME %s", $1); }
    | INTNUM 		{ emit("NUMBER %d", $1); }
    | expr '+' expr 	{ emit("ADD"); }
    | expr '-' expr 	{ emit("SUB"); }
    | expr '*' expr 	{ emit("MUL"); }
    | expr '/' expr 	{ emit("DIV"); }
    ;
 ```
When it parses `a+2*3`, it emits NAME a, NUMBER 2, NUMBER 3, MUL, ADD.

4. There are a few character tokens like ';' that aren’t operators and so have no prece-
dence that didn’t have to be defined.

5. bison’s precedence rules normally use the precedence of the rightmost token in a rule

6. Default action $$ = $1 for rules with no explicit action.

7. Bison starts the numbers for named tokens at 258
