calc: calc.y calc.l calc.h funcs.c
	bison -d calc.y
	flex calc.l
	gcc -o $@ lex.yy.c calc.tab.c funcs.c -lfl -lm -g


.PHONY: claen
clean:
	rm lex.yy.c *.tab.* calc
