# flex & bison SEL

## Flex

1. a(bc|de) matches abc or ade. 

2. 0/1 matches 0 in the string 01 but would not match anything in the string 0 or 02. 
The material matched by the pattern following the slash is not “consumed” and remains
to be turned into subsequent tokens. 

3. How Flex Handles Ambiguous Patterns
* Match the longest possible string every time the scanner matches input.
* In the case of a tie, use the pattern that appears first in the program.

4. the three levels of input management are:
* Setting yyin to read the desired file(s)
* Creating and using YY\_BUFFER\_STATE input buffers
* Redefining YY\_INPUT

5. Harking back to the earliest versions of lex, unless you tell it
otherwise, flex acts as though there is a default rule at the end of the scanner that copies
otherwise unmatched input to yyout.
```
.	ECHO;

#define ECHO fwrite( yytext, yyleng, 1, yyout )
```
`%option nodefault` at the top of the scanner to tell it not to add a default rule and rather
to report an error if the input rules don’t cover all possible input. 


## Bison
