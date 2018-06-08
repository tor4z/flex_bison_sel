# flex & bison SEL

## Flex

1. a(bc|de) matches abc or ade. 

2. 0/1 matches 0 in the string 01 but would not match anything in the string 0 or 02. 
The material matched by the pattern following the slash is not “consumed” and remains
to be turned into subsequent tokens. 

3. How Flex Handles Ambiguous Patterns
* Match the longest possible string every time the scanner matches input.
* In the case of a tie, use the pattern that appears first in the program.


## Bison
